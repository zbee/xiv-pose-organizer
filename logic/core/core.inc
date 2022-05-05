<?php

class pose_organizer extends structure_swap {
  public int    $work_through_step      = step_configure;
  public int    $work_through_pose_step = 0;
  public string $dots                   = '../';

  /**
   * @throws Exception
   */
  public function __construct(
    string $dots = '../',
    bool   $prevent_checksum = false
  ) {
    $this->dots = $dots;
    if (!is_dir($dots . 'data')) {
      if (
        !mkdir($concurrentDirectory = $dots . 'data') &&
        !is_dir($concurrentDirectory)
      ) {
        throw new RuntimeException('Data directory was not created');
      }
    }
    else if (file_exists($dots . 'data/resume.json')) {
      $resume_at = file_get_contents($dots . 'data/resume.json');
      $resume_at = (int)$resume_at;
      $this->parse_folder(false, !$prevent_checksum);
      $this->work_through_pose_step = $resume_at;
      $this->work_through_step      = step_work_through;
    }
    else if (file_exists($dots . 'data/settings.json')) {
      $this->work_through_step = step_configure_review;
    }
    else {
      $this->work_through_step = step_configure;
    }
  }
}