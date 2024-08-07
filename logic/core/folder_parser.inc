<?php /** @noinspection PhpPossiblePolymorphicInvocationInspection */

use JetBrains\PhpStorm\ArrayShape;

class pose_folder_parser extends settings {

  /**
   * @var DirectoryIterator $folders pointer for the folder current parsing
   */
  private DirectoryIterator $folders;

  /**
   * @var pose[] $poses list of parsed poses
   */
  public array $poses;

  /**
   * @var string $poses_checksum checksum of the poses available
   */
  private string $poses_checksum;

  /**
   * @param bool $reparse whether the current data available should be
   *                      overwritten
   *                      Just check settings and start parsing the poses
   *
   * @throws RuntimeException|JsonException
   */
  public function parse_folder(
    bool $reparse = false,
    bool $prevent_checksum = false
  ): void {
    parent::__construct();

    if (!is_dir($this->poses_folder)) {
        throw new RuntimeException('Configured poses folder could not be found.');
    }

    $this->folders = new DirectoryIterator($this->poses_folder);

    // If available load the saved poses data
    $check_checksum = false;
    if (file_exists($this->dots . 'data/poses.json')) {
        $pose_data = json_decode(
            file_get_contents($this->dots . 'data/poses.json'),
            false,
            512,
            JSON_THROW_ON_ERROR
        );

      foreach ($pose_data->poses as $key => $pose) {
        $pose_data->poses[$key] = new pose($pose);
      }

      $this->poses_checksum = $pose_data->checksum;
      $this->poses          = $pose_data->poses;
      usort($this->poses, static fn($a, $b) => strcmp($a->path, $b->path));

      if (!$prevent_checksum) {
        $check_checksum = $this->poses_checksum(manual_check: true);
      }
    }

    // Reparse the poses folder if it has not yet been, if reparse is specified,
    //    or if the checksum has changed
      /** @noinspection NotOptimalIfConditionsInspection */
    if (
      $reparse ||
      !file_exists($this->dots . 'data/poses.json') ||
      (!$prevent_checksum && $check_checksum !== $this->poses_checksum)
    ) {
      $temporary_poses = false;
      if (isset($this->poses) && count($this->poses) > 0) {
        $temporary_poses = $this->poses;
      }
      $this->poses          = [];
      $this->poses_checksum =
        $this->poses_checksum(overwrite_poses: $temporary_poses);
      $this->save_poses();
    }
  }

  /**
   * @param string $name name of the pose to normalize
   *                     This will replace dashes that are used instead of
   *                     spaces and strip to just expanded ascii characters
   *
   * @return string normalized pose name
   */
    private function normalize_pose_name(string $name): string
    {
    // Swap underscores and hyphens that are in place of spaces with
    //  actual spaces
    if (substr_count($name, '_') > 0 && substr_count($name, ' ') === 0) {
      $name = str_replace('_', ' ', $name);
    }
    if (substr_count($name, '-') > 0 && substr_count($name, ' ') === 0) {
      $name = str_replace('-', ' ', $name);
    }

    //Remove extra characters beyond expanded ascii
    return preg_replace('/[[:^print:]]/', '', $name);
  }

  /**
   * @param DirectoryIterator|bool $folder          folder to parse, used for
   *                                                recursion
   * @param pose[]|bool            $overwrite_poses list of poses to save the
   *                                                data from
   *
   * @return int the number of poses found in the folder
   */
  private function parse_contents(
    DirectoryIterator|bool $folder = false,
    array|bool             $overwrite_poses = false
  ): int {

    // If we're not recurring into a folder, the target is the configured
    //    pose folder
    if ($folder === false) {
      $folder = $this->folders;
    }

    // Keep track of the number of poses found
    $poses = 0;
    // If this is a recursive call, grab the number of poses
    if ($folder !== false) {
      $nearby_file = new DirectoryIterator($folder->getPath());
      foreach ($nearby_file as $possible_pose) {
        if (
          str_ends_with($possible_pose->getFilename(), '.pose') ||
          str_ends_with($possible_pose->getFilename(), '.cmp')
        ) {
          $poses++;
        }
      }
    }

    // Loop through the discovered files and folders within the target
    foreach ($folder as $found) {
      // Ignore dots
      if ($found->isDot()) {
        continue;
      }
      // Recurse into folders
      if ($found->isDir()) {
        // Don't search the working directory
          if ($folder->getFilename() === '..working') {
          continue;
        }

        $poses += $this->parse_contents(
          new DirectoryIterator(
            $found->getPath() . '\\' . $found->getFilename(),
          ),
          $overwrite_poses
        );
        continue;
      }

      // If it's a file
      $is_pose = false;

      //region Determine file format, and if it is a pose
      $file      = explode('.', $found->getFilename());
      $extension = strtolower($file[count($file) - 1]);
      unset($file[count($file) - 1]);
      $file_name   = implode('.', $file);
      $file_format = null;
        if ($extension === 'pose') {
        $file_format = 'ana';
        $is_pose     = true;
      }
        if ($extension === 'cmp') {
        $file_format = 'cmt';
        $is_pose     = true;
      }

      $low_file_name = strtolower($file_name);
      $pose_name     = $this->normalize_pose_name($low_file_name);
      //endregion Determine file format

      if (!$is_pose) {
        continue;
      }

      //region Check if pose exists
      //Hashing the parse file to generate a key to track it into the reworked
      //    pose folder
      $pose_data = file_get_contents($found->getRealPath());
      $key       = md5($pose_data);

      // If temporary previous pose data exists for overwriting
      if ($overwrite_poses !== false) {
        $overwrite_found = false;
        // Loop through the overwrite poses
        foreach ($overwrite_poses as $overwrite_key => $overwrite_pose) {
          // Compare keys
          if ($overwrite_pose->key === $key) {
            // Save the pose
            ++$poses;
            $this->poses[]   = $overwrite_pose;
            $overwrite_found = true;
            // Un-save this overwrite pose
            unset($overwrite_poses[$overwrite_key]);
          }
        }
        // Skip this pose if a match was found
        if ($overwrite_found) {
          continue;
        }
      }

      //endregion Check if pose exists
      //region Parsing a pose

      //region Checking nearby for a preview image
      $image_name  = null;
      $nearby_file = new DirectoryIterator($found->getPath());
      foreach ($nearby_file as $possible_preview) {
        // Ignore dots
        if ($possible_preview->isDot()) {
          continue;
        }
        if (
            !$possible_preview->isDir() &&
            str_contains($possible_preview->getFilename(), '.')
        ) {
          //region Determine file extension and name
          $extension = explode('.', $possible_preview->getFilename());
          $extension = strtolower($extension[count($extension) - 1]);

          $file_name     = $possible_preview->getFilename();
          $file_name     = str_replace(".$extension", '', $file_name);
          $low_file_name = strtolower($file_name);
          //endregion Determine file extension and name

            if ($extension === 'jpg' || $extension === 'png') {
            $image_name = $possible_preview->getRealPath();
            $image_name = str_replace($this->poses_folder, '', $image_name);
            break;
          }
        }
      }
      //endregion Checking nearby for a preview image

      $extract   = $this->extract_race_and_gender($pose_name);
      $gender    = $extract['gender'];
      $pose_race = $extract['race'];
      $pose_name = $extract['name'];

      //region Check for author
      $author = null;
      // If author name is bracketed, assume the name is within those brackets
      if (str_contains($pose_name, '[')) {
        $author = explode('[', $pose_name);
        $author = explode(']', $author[count($author) - 1])[0];
        // Remove the author name from the file name
        $pose_name = str_replace("[$author]", '', $pose_name);
      }
      // If there is  ' by X' in the file name, assume author name follows it
      else if (str_contains($pose_name, ' by ')) {
        $author = explode(' by ', $pose_name)[1];
        $author = explode(' ', $author)[0];
        // Remove the author name and the identifier from the file name
        $pose_name = str_replace(" by $author", '', $pose_name);
      }
      //endregion Check for author

      // Clean up extra spaces from grabbing pose attributes
      $pose_name = preg_replace('/\s+/', ' ', trim($pose_name));
      //endregion Parsing a pose

      try {
        ++$poses;
        $this->poses[] = new pose(
          name:        $pose_name,
          key:         $key,
          file_format: $file_format,
          path:        $found->getRealPath(),
          pack:        $poses > 1,
          author:      $author,
          image_name:  $image_name,
          race:        $pose_race,
          gender:      $gender,
        );
      } catch (Exception) {
      }
    }

    return $poses;
  }

  /**
   * A method to unlink duplicate poses, and save the path to the file on the
   * pose that is desired.
   * Note: This is not a method for pruning poses that are not of desired
   * formats, only to take them out of the work_through process.
   *
   * @param string $pose_name The name of the pose, simply without the extension
   * @param string $path      The full path to the pose file
   *
   * @return void
   */
  private function prune_duplicate_poses_from_work(
    string $pose_name,
    string $path
  ): void {
    // Remove the file extension from the path, and save it
    $root_path = explode('.', $path);
    $extension = $path[count($root_path) - 1];
    unset($root_path[count($root_path) - 1]);
    // Remove the file name from the path, and save it
    $root_path = implode('.', $root_path);
    $root_path = explode('\\', $root_path);
    unset($root_path[count($root_path) - 1]);
    $root_path = implode('\\', $root_path);
    // Lower case the path
    $root_path_case = $root_path;
    $root_path      = strtolower($root_path);
    $pose_name_case = $pose_name;
    $pose_name      = strtolower($pose_name);

    // Determine the format of the extension
    $file_format = 'ana';
    if ($extension === 'cmp') {
      $file_format = 'cmt';
    }

    // Determine the file extension for the other format
    $other_format = 'cmt';
    if ($this->format_for_pose_normalization === 'cmt') {
      $other_format = 'ana';
    }
    $other_extension = constant('file_extension_' . $other_format);

    // Determine if this file or the other is the duplicate (based on config)
    if ($file_format === $this->format_for_pose_normalization) {
      $which_duplicate = other_file;
    }
    else {
      $which_duplicate = this_file;
    }

    // Determine the two file paths we will be looking for
    if ($which_duplicate === other_file) {
      $good_path      = $path;
      $duplicate_path =
        $root_path_case . '\\' . $pose_name_case . '.' . $other_extension;
    }
    else {
      $good_path      = $root_path . '\\' . $pose_name . '.' . $other_extension;
      $duplicate_path = $path;
    }

    // Find the two pose structs in the list
    foreach ($this->poses as $key => $pose) {
      // Save the duplicate path if this pose is the one we want to keep
      if (strtolower($pose->path) === strtolower($good_path)) {
        $this->poses[$key]->duplicate_pose = $duplicate_path;
      }
      // Unlink the duplicate pose
      if (strtolower($pose->path) === strtolower($duplicate_path)) {
        unset($this->poses[$key]);
      }
    }
  }

  /**
   * Method to save all of the poses to a json file in the data folder
   * @throws JsonException
   */
  protected function save_poses(): void {
    $pose_names = [];
    foreach ($this->poses as $pose) {
      // Not standard method of getting a name but will allow for more collision
      //    without prompting it (only difference between poses in a
      //    pack may only be differentiated by race, etc)
      $pose_name = explode('\\', strtolower($pose->path));
      $pose_name = explode('.', $pose_name[count($pose_name) - 1]);
      unset($pose_name[count($pose_name) - 1]);
      $pose_name = implode('.', $pose_name);

      // If the name already exists, have it pruned
      if (isset($pose_names[$pose_name])) {
        $this->prune_duplicate_poses_from_work($pose_name, $pose->path);
      }
      // Save the file names without extension to list
      else {
        $pose_names[$pose_name] = 0;
      }
    }

    usort($this->poses, static fn($a, $b) => strcmp($a->path, $b->path));

    file_put_contents(
      '../data/poses.json',
      json_encode(
        [
          'checksum' => $this->poses_checksum,
          'poses'    => $this->poses,
        ],
          JSON_THROW_ON_ERROR
      )
    );
  }

  /**
   * @param string $pose_name
   *
   * @return string[] list of pose name, race, and gender
   */
  #[ArrayShape([
    'name'   => "string",
    'race'   => "mixed|null",
    'gender' => "bool|null",
  ])]
  protected function extract_race_and_gender(string $pose_name): array {
    $pose_name = strtolower($pose_name);

    //region Determine race
    $pose_race = null;
    //Step through each race's list of aliases
    foreach (races as $race) { // List of race constants
      foreach (constant("race_$race") as $race_race) // List of race aliases
      {
        if (str_contains($pose_name, $race_race)) {
          $pose_race = constant("race_$race")[0];
          // Add an indicator of where the race was
          $pose_name = str_replace($race_race, '*$@', $pose_name);
          break;
        }
      }
        if ($pose_race !== null) {
        break;
      }
    }
    //endregion Determine race

    //region Check for gender
    $gender = null;
    // Check for commonly occurring m or f next to the race
      if ($pose_race !== null) {
      if (str_contains($pose_name, ' m ')) {
        $pose_name = str_replace(' m ', 'male', $pose_name);
      }
      else if (str_contains($pose_name, '*$@m')) {
        $pose_name = str_replace('*$@m', 'male', $pose_name);
      }
      else if (str_contains($pose_name, ' f ')) {
        $pose_name = str_replace(' f ', 'female', $pose_name);
      }
      else if (str_contains($pose_name, '*$@f')) {
        $pose_name = str_replace('*$@f', 'female', $pose_name);
      }
    }
    // Check for also-common female or male specification
    if (str_contains($pose_name, 'male')) {
      $gender = gender_male;
      if (str_contains($pose_name, 'female')) {
        $gender = gender_female;
      }

      // Trim the gender from the file name
      $pose_name = str_replace(['female', 'male'], '', $pose_name);
    }
    // Strip race indicator
    $pose_name = str_replace('*$@', '', $pose_name);
    //endregion Check for gender

    return [
      'name'   => $pose_name,
      'race'   => $pose_race,
      'gender' => $gender,
    ];
  }

  /**
   * This method generates a checksum of the poses
   *
   * @param bool                   $manual_check    whether it should check the
   *                                                files or the parsed poses
   * @param DirectoryIterator|bool $folder          only to be used with a true
   *                                                $manual_check, is the
   *                                                target
   *                                                for recursion
   * @param pose[]|bool            $overwrite_poses worked-through poses for
   *                                                overwriting name value
   *                                                purposes
   */
  private function poses_checksum(
    bool                   $manual_check = false,
    DirectoryIterator|bool $folder = false,
    array|bool             $overwrite_poses = false
  ): string {
    $first_level = false;
    $check       = '';
    if (!$manual_check) {
      $first_level = true;
      foreach ($this->poses as $pose) {
        $check .= substr($pose->name, 0, 3);
      }
    }
    else {
      // Use the same folder search, recursion, and pose naming as
      //    $this->parse_contents()
      if ($folder === false) {
        $first_level = true;
        $folder      = $this->folders;
      }
      foreach ($folder as $found) {
        if ($found->isDot()) {
          continue;
        }
        if ($found->isDir()) {
          // Don't search the working directory
            if ($folder->getFilename() === '..working') {
            continue;
          }
          $check .= $this->poses_checksum(
            true,
            new DirectoryIterator(
              $found->getPath() . '\\' . $found->getFilename()
            ),
            $overwrite_poses
          );
          continue;
        }

        $is_pose   = false;
        $file      = explode('.', $found->getFilename());
        $extension = strtolower($file[count($file) - 1]);
        unset($file[count($file) - 1]);
        $file_name = implode('.', $file);
          if ($extension === file_extension_ana) {
          $is_pose = true;
        }
          if ($extension === file_extension_cmt) {
          $is_pose = true;
        }
        $low_file_name = strtolower($file_name);
        $pose_name     = $this->normalize_pose_name($low_file_name);

        if (!$is_pose) {
          continue;
        }

        // If temporary previous pose data exists for overwriting
        if ($overwrite_poses !== false) {
          $pose_data       = file_get_contents($found->getRealPath());
          $key             = md5($pose_data);
          $overwrite_found = false;
          // Loop through the overwrite poses
          foreach ($overwrite_poses as $overwrite_pose) {
            // Compare keys
            if ($overwrite_pose->key === $key) {
              $overwrite_found = true;
              $pose_name       = $overwrite_pose->name;
            }
          }
          // Skip this pose if a match was found
          if ($overwrite_found) {
            $check .= substr($pose_name, 0, 3);
            continue;
          }
        }

        $pose_name = $this->extract_race_and_gender($pose_name)['name'];

        if (str_contains($pose_name, '[')) {
          $author    = explode('[', $pose_name);
          $author    = explode(']', $author[count($author) - 1])[0];
          $pose_name = str_replace("[$author]", '', $pose_name);
        }
        else if (str_contains($pose_name, ' by ')) {
          $author    = explode(' by ', $pose_name)[1];
          $author    = explode(' ', $author)[0];
          $pose_name = str_replace(" by $author", '', $pose_name);
        }
        unset($author);
        $pose_name = preg_replace('/\s+/', ' ', trim($pose_name));
        $check     .= substr($pose_name, 0, 3);
      }
    }

    if ($first_level) {
      $check = dechex(crc32($this->poses_folder . $check));
    }
    return $check;
  }
}
