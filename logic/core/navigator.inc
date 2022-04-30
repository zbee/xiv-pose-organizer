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
    for ($i = 0; $i < count($this->poses); $i++) {
      if ($this->poses[$i]->worked_through !== true) {
        $this->work_through_pose_step = $i;
        file_put_contents('../data/resume.json', (int)$i);
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
    if ($resume !== false)
      $pose = $resume;

    // Return early if the key is requested
    if ($key)
      return $pose;

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
    return floor($this->work_through_pose_step / $this->pose_count()) * 100;
  }

  /**
   * @return string[] the list of packs that have been used before
   */
  public function get_packs(): array{
    if (!file_exists('../data/packs.json')) {
      file_put_contents('../data/packs.json', json_encode([]));
      $this->packs = [];
    }
    else {
      $this->packs = json_decode(file_get_contents('../data/packs.json'), true);
    }
    return $this->packs;
  }

  public function find_pack($identifier): string {
    if ($identifier !== '') {
      foreach ($this->packs as $key => $pack) {
        if ($key === $identifier)
          return $pack;
        elseif ($pack === $identifier)
          return $key;
      }
    }
  }

  /**
   * @param array $packs list of pack names that have been used before
   */
  public function set_packs(array $packs): void {
    file_put_contents('../data/packs.json', json_encode($packs));
    $this->packs = $packs;
  }
}