<?php /** @noinspection PhpPossiblePolymorphicInvocationInspection */

class navigator extends maintain_tagging {

  /**
   * @var string[] $packs a list of packs, key=>pack name
   */
  public array $packs;

  /**
   * Method to search through the list of the poses and reset the work_through
   * pose step to the first one that is incomplete
   * @return int of the new step
   */
  public function find_first_incomplete_pose(): int {
    for ($i = 0, $iMax = count($this->poses); $i < $iMax; $i++) {
      if ($this->poses[$i]->worked_through !== true) {
        $this->work_through_pose_step = $i;
        file_put_contents('../data/resume.json', $i);
        return $i;
      }
    }
    file_put_contents('../data/resume.json', 0);
    return 0;
  }

  /**
   * Method to return the pose object for the current step
   * @return pose|int the current pose, or the key to the current pose
   */
  public function current_pose($resume = false, bool $key = false): pose|int {
    // Determine which pose to return
    $pose = $this->work_through_pose_step;
    if ($resume !== false) {
      $pose = $resume;
    }

    // Return early if the key is requested
    if ($key) {
      return $pose;
    }

    return $this->poses[$pose];
  }

  /**
   * @return int the quantity of poses that there are in the workload
   */
  public function pose_count(): int {
    return count($this->poses);
  }

  /**
   * @return int the percentage of progress through the workload
   */
  public function progress_bar_percentage(): int {
    return round($this->work_through_pose_step / $this->pose_count() * 100);
  }

  /**
   * @return string[] the list of packs that have been used before
   */
  public function get_packs(): array {
    if (!file_exists('../data/packs.json')) {
      file_put_contents('../data/packs.json', json_encode([]));
      $this->packs = [];
    }
    else {
      $this->packs = json_decode(file_get_contents('../data/packs.json'), true);
    }
    return $this->packs;
  }

  /**
   * @param string $identifier key or name of pack to find
   *
   * @return string return key or name (opposite of input) of pack
   */
  public function find_pack(string $identifier): string {
    foreach ($this->packs as $key => $pack) {
      if ($key === $identifier) {
        return $pack;
      }

      if ($pack === $identifier) {
        return $key;
      }
    }

    return '';
  }

  /**
   * @param array $packs list of pack names that have been used before
   */
  public function set_packs(array $packs): void {
    file_put_contents('../data/packs.json', json_encode($packs));
    $this->packs = $packs;
  }

  /**
   * @param string $copy_type whether to copy the last pose or the first pose
   *                          that has a similar name
   *
   * @return pose
   * @throws RuntimeException if invalid copy type is given
   */
  public function copy_pose_fields(string $copy_type): pose {
    if ($copy_type !== copy_first && $copy_type !== copy_last) {
      throw new RuntimeException('copy_pose_fields: invalid copy_type');
    }

    $current_pose_index = $this->current_pose(key: true);

    //region Copy first of name
    if ($copy_type === copy_first) {
      $pose = $this->current_pose();

      //region Same name and path
      // First look for a pose with the same file name, minus race and gender
      $pose_path = explode('\\', $pose->path);
      $pose_path = $pose_path[count($pose_path) - 1];
      $pose_path = $this->extract_race_and_gender($pose_path)['name'];

      foreach ($this->poses as $index => $match) {
        if ($index >= $current_pose_index) {
          continue;
        }
        $match_path = explode('\\', $match->path);
        $match_path = $match_path[count($match_path) - 1];
        $match_path = $this->extract_race_and_gender($match_path)['name'];
        if (
          $match->key !== $pose->key && $match_path === $pose_path
        ) {
          return $match;
        }
      }
      //endregion Same name and path

      //region Same path
      // If a same-name pose wasn't found grab the first one from the same
      //   folder
      $pose_path = explode('\\', $pose->path);
      unset($pose_path[count($pose_path) - 1]);
      $pose_path = implode('\\', $pose_path);
      foreach ($this->poses as $index => $match) {
        if (
          $index < $current_pose_index &&
          $match->key !== $pose->key &&
          str_starts_with($match->path, $pose_path)
        ) {
          return $match;
        }
      }
      //endregion Same path

      //region Same name and one folder up
      // If a same-folder search didn't yield a pose previous to the current
      //   one, try to search by name again one folder up
      $pose_path = explode('\\', $pose->path);
      unset(
        $pose_path[count($pose_path) - 1], $pose_path[count($pose_path) - 1]
      );
      $pose_path = implode('\\', $pose_path);
      $pose_name = explode('\\', $pose->path);
      $pose_name = $pose_name[count($pose_name) - 1];
      $pose_name = $this->extract_race_and_gender($pose_name)['name'];
      foreach ($this->poses as $index => $match) {
        if ($index >= $current_pose_index) {
          continue;
        }
        $match_name = explode('\\', $match->path);
        $match_name = $match_name[count($match_name) - 1];
        $match_name = $this->extract_race_and_gender($match_name)['name'];
        if (
          $match->key !== $pose->key &&
          $match_name === $pose_name &&
          str_starts_with($match->path, $pose_path)
        ) {
          return $match;
        }
      }
      //endregion Same name and one folder up

      //region One folder up
      // If a same-folder search didn't yield a pose previous to the current
      //   one, try to search one folder up
      $pose_path = explode('\\', $pose->path);
      unset(
        $pose_path[count($pose_path) - 1], $pose_path[count($pose_path) - 1]
      );
      $pose_path = implode('\\', $pose_path);
      foreach ($this->poses as $index => $match) {
        if (
          $index < $current_pose_index &&
          $match->key !== $pose->key &&
          str_starts_with($match->path, $pose_path)
        ) {
          return $match;
        }
      }
      //endregion One folder up

      return new pose('empty');
    }
    //endregion Copy first of name

    return $this->poses[$current_pose_index - 1];
  }
}