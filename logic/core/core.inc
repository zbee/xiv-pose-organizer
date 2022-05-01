<?php

class pose_organizer extends structure_swap {
  public int    $work_through_step      = step_configure;
  public int    $work_through_pose_step = 0;
  public string $dots                   = '../';

  public function __construct($dots = '../') {
    $this->dots = $dots;
    if (!is_dir($dots . 'data'))
      mkdir($dots . 'data');
    else {
      if (file_exists($dots . 'data/resume.json')) {
        $resume_at = file_get_contents($dots . 'data/resume.json');
        $resume_at = (int)$resume_at;
        $this->parse_folder();
        $this->work_through_pose_step = $resume_at;
        $this->work_through_step = step_work_through;
      } else {
        if (file_exists($dots . 'data/settings.json'))
          $this->work_through_step = step_configure_review;
        else
          $this->work_through_step = step_configure;
      }
    }
  }
}