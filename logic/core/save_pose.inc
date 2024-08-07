<?php
/** @noinspection CascadeStringReplacementInspection */

/** @noinspection PhpInconsistentReturnPointsInspection */

class save_pose extends navigator {

  private function find_pose(string $key): pose {
      /** @noinspection ForeachInvariantsInspection */
    for ($i = 0, $iMax = count($this->poses); $i < $iMax; $i++) {
      if ($this->poses[$i]->key == $key) {
        return $this->poses[$i];
      }
    }
  }

    /**
     * @throws JsonException
     */
  private function save_tracked(array $tracked_data): void {
    $this->set_tag_data(
      $tracked_data['tags'],
      $tracked_data['categories'],
      $tracked_data['verbs'],
    );
    $this->set_packs($tracked_data['packs']);
  }

  private function create_link(string $url, string $file): void {
    $content = "[InternetShortcut]\r\nURL=" . $url . "\r\n";
      $fp = fopen($file . '.url', 'wb');
    fwrite($fp, $content);
    fclose($fp);
  }

  private function write_pose(pose $pose): void {
    // Create the working directory if it doesn't exist.
    $destination = $this->poses_folder . '\\..working\\';
    if (
      !file_exists($destination) &&
      !mkdir($destination) &&
      !is_dir($destination)
    ) {
      throw new RuntimeException('Working directory was not created');
    }

    // Grab the linked file extensions
    if ($pose->has_preview) {
      $image_extension = explode('.', $pose->image_name);
      $image_extension = '.' . $image_extension[count($image_extension) - 1];
    }
    if ($pose->duplicate_pose !== null) {
      $duplicate_extension = explode('.', $pose->duplicate_pose);
      $duplicate_extension =
        $duplicate_extension[count($duplicate_extension) - 1];
    }

    // Handle skipped data
    if ($pose->author === 'n/a') {
      $pose->author = null;
    }
    if ($pose->link === 'n/a') {
      $pose->link = null;
    }

    //region Folder name
    // Choose fill data for folder names
    // Pose name
    $folder_pose_name = $pose->name;
    if ($pose->pack) {
      $folder_pose_name = $this->find_pack($pose->pack_name);
      if (!$this->file_and_folder_separate_tagging) {
        $folder_pose_name .= ';' . $pose->name;
      }
    }

    // Categories
    $category_one = $pose->categories[0] ?? '';
    $category_two = $pose->categories[1] ?? '';
    $category_thr = $pose->categories[2] ?? '';

    // Tags
    $tags = '';
    if (!$this->file_and_folder_separate_tagging) {
      foreach ($pose->tags as $tag) {
        $tags .= $tag . ' ';
      }
    }

    // Author
    $author = '';
    if ($pose->author !== null) {
      $author = $this->author_format;
      $author = str_replace('%author%', $pose->author, $author);
    }

    // Others posed
    $group = '';
    if ($pose->other_people_posed > 0) {
      $group = 'g(' . $pose->other_people_posed + 1 . ')';
    }

    // Fill in folder name from format
    $folder = $this->folder_name_format;
    $folder = str_replace('%category_1%', $category_one, $folder);
    $folder = str_replace('%category_2%', $category_two, $folder);
    $folder = str_replace('%category_3%', $category_thr, $folder);
    $folder = str_replace('%tags%', $tags, $folder);
    $folder = str_replace('%group%', $group, $folder);
    $folder = str_replace('%author%', $author, $folder);
    $folder = str_replace('%pose_name%', $folder_pose_name, $folder) . '\\';
    $folder = preg_replace('/\s+/', ' ', trim($folder));
    $folder = str_replace(' }', '}', $folder);
    //endregion Folder name

    //region File name
    // Choose fill data for file names
    $file_name = $this->file_name_format;

    // Fill in categories
    $categories = ['', '', ''];
    // Fill in each category
    foreach ($pose->categories as $index => $category) {
      if (
        !$this->file_and_folder_separate_tagging ||
        !$this->use_nested_structure_for_poses
      ) {
        $categories[$index] = $category . ' ';
      }
      else {
        $categories[$index] = $category[0];
      }
    }

    //Format categories
    $categories_text = $this->category_format;
    foreach ($categories as $index => $category) {
      ++$index;
      $categories_text = str_replace(
        "%category_$index%",
        $category,
        $categories_text
      );
    }

    // Fill in tags
    $tags = ['', '', ''];
    // Fill in each tag
    foreach ($pose->tags as $index => $tag) {
      $tags[$index] = $tag;
    }

    // Format tags
    foreach ($tags as $index => $tag) {
      ++$index;
      $file_name = str_replace(
        "%tag_$index%",
        $tag,
        $file_name
      );
    }

    // Author
    $author = $this->author_format;
    if (
      !$this->always_attribute_to_author &&
      $this->use_nested_structure_for_poses
    ) {
      $author = str_replace('%author%', '', $author);
    }
    else if ($pose->author === null) {
      $author = '';
    }
    else {
      $author = str_replace('%author%', $pose->author, $author);
    }

    // Pose name, pack name
    $file_pose_name = $pose->name;
    if (!$this->use_nested_structure_for_poses && $pose->pack) {
      $file_pose_name = $this->find_pack($pose->pack_name) . ';' . $pose->name;
    }

    // Gender
    $gender = 'female';
    if ($pose->gender == gender_male) {
      $gender = 'male';
    }

    // Others posed
    $sub = '';
    // Group, if structure is flat
    if (
      $pose->other_people_posed > 0 && !$this->use_nested_structure_for_poses
    ) {
      $sub .= 'g(' . $pose->other_people_posed + 1 . ') ';
    }
    // Sub/Dom people required
    $pose->submission_to_other === true ? $sub .= 'sub' : $sub .= '';
    $pose->submission_to_other === false ? $sub .= 'dom' : $sub .= '';
    if ($pose->other_people_required > 0) {
      if ($pose->submission_to_other === null) {
        $sub .= 'needs';
      }
      $sub .= '(' . $pose->other_people_required . ')';
    }

    // Fill in file name from format
    $file_name      = str_replace('%categories%', $categories_text, $file_name);
    $file_name      = str_replace('%author%', $author, $file_name);
    $file_name      = str_replace('%pose_name%', $file_pose_name, $file_name);
    $file_name      = str_replace('%gender%', $gender, $file_name);
    $file_name      = str_replace('%sub%', $sub, $file_name);
    $file_name      = str_replace('%verb%', $pose->verb, $file_name);
    $file_name      = str_replace('%race%', $pose->race, $file_name);
    $file_name      = preg_replace('/\s+/', ' ', trim($file_name));
    $file_name      = str_replace(' }', '}', $file_name);
    $file_extension = '.' . constant('file_extension_' . $pose->file_format);
    //endregion File name

    //region Loose files
    // Write corrected files to working directory
    if (!$this->use_nested_structure_for_poses) {
      // Copy Pose files
      // Pose
      copy($pose->path, $destination . $file_name . $file_extension);
      // Duplicated pose
      if (
          $this->normalize_poses &&
          $this->keep_other_pose_formats &&
        $pose->duplicate_pose !== null
      ) {
        copy(
          $pose->duplicate_pose,
          $destination . $file_name . '.' . $duplicate_extension
        );
      }

      // Copy auxiliary files
      // Link
      if ($this->want_pose_link && $pose->link !== null) {
        $this->create_link($pose->link, $destination . $file_name);
      }
      // Image
      if ($this->want_pose_previews && $pose->has_preview) {
        // Support pose previews and uploaded previews
        $image = '\\' . $pose->image_name;
        if (str_contains($image, 'assets\\tmp\\')) {
          $image =
            str_replace('\\assets\\tmp\\', '..\\web\\assets\\tmp\\', $image);
        }
        else {
          $image = $this->poses_folder . '\\' . $image;
        }

        copy(
          $image,
          $destination . $file_name . $image_extension
        );
      }
    }
    //endregion Loose files
    //region Foldered files
    // Write corrected folders and files to working directory
    else {
      // Copy Pose files
      // Pose
      if (
        !file_exists($destination . $folder) &&
        !mkdir($concurrentDirectory = $destination . $folder) &&
        !is_dir($concurrentDirectory)
      ) {
        throw new RuntimeException(
          sprintf('Pose directory "%s" was not created', $concurrentDirectory)
        );
      }
      copy($pose->path, $destination . $folder . $file_name . $file_extension);
      // Duplicated pose
      if (
          $this->normalize_poses &&
          $this->keep_other_pose_formats &&
        $pose->duplicate_pose !== null
      ) {
        copy(
          $pose->duplicate_pose,
          $destination . $folder . $file_name . '.' . $duplicate_extension
        );
      }

      // Copy auxiliary files
      // Link
      if ($this->want_pose_link && $pose->link !== null) {
        $this->create_link($pose->link, $destination . $folder . 'link');
      }
      // Image
      if ($this->want_pose_previews && $pose->has_preview) {
        // Support pose previews and uploaded previews
        $image = '\\' . $pose->image_name;
        if (str_contains($image, '\\assets\\tmp\\')) {
          $image = str_replace('\\..', '..\\web', $image);
        }
        else {
          $image = $this->poses_folder . '\\' . $image;
        }


        copy(
          $image,
          $destination . $folder . 'preview' . $image_extension
        );
      }
    }
    //endregion Foldered files
  }

  /**
   * @throws Exception
   */
    public function save(array $post_data): void
    {
    if (!isset($post_data['_categories'])) {
      return;
    }

    //region Tracked data
    if (!isset($post_data['pack_names'])) {
      $post_data['pack_names'] = [];
    }
    // Save tracked data (tags, cats, verbs, packs)
    $this->save_tracked(
      [
        'categories' => $post_data['_categories'],
        'tags'       => $post_data['_tags'],
        'verbs'      => $post_data['verbs'],
        'packs'      => $post_data['pack_names'],
      ]
    );
    // Discard tracked data
        unset(
            $post_data['_categories'],
            $post_data['_tags'],
            $post_data['verbs'],
            $post_data['pack_names']
        );
    //endregion Tracked data

    // Clean up pack-dependent data
    if ($post_data['pack'] === '0') {
      $post_data['pack_name']             = null;
      $post_data['other_people_posed']    = 0;
      $post_data['other_people_required'] = 0;
      $post_data['submission_to_other']   = null;
    }
    else {
      $post_data['submission_to_other'] =
        $post_data['submission_to_other'] == '' ? null
          : $post_data['submission_to_other'] == '1';
    }

    // Find the pose
    $pose = $this->find_pose($post_data['key']);
    // Update the pose data
    $pose->save($post_data);
    // Save $this->poses
    $this->save_poses();
    // Write the pose to the working directory
    $this->write_pose($pose);
    // Refresh poses
    $this->parse_folder();
  }

    public function skip($key): void
    {
    $pose = $this->find_pose($key);
    $pose->skip();
    $this->save_poses();
  }

}
