<?php
require '../logic/autoload.php';
$poser = new pose_organizer();

//region Handle a pose folder on the step of configuring settings
if (
  isset($_GET['configure']) or
  isset($_GET['configure_tags']) or
  isset($_GET['configure_review'])
) {
  require 'templates/configure.php';
}

if (isset($_POST['configure']) or isset($_POST['configure_tags'])) {
  require 'handle/configure.php';
}
//endregion Handle a pose folder on the step of configuring settings

//region Handle work_through steps, with a pose number
if (isset($_GET['work_through'])) {
  //Setup the file to hold the step in the work_through process
  file_put_contents('../data/resume.json', $_GET['resume'] ?? 0);
  $poser->parse_folder();

  require 'templates/work_through.php';
}

if (isset($_POST['work_through']) and isset($_POST['resume'])) {
  //Update the step in the work_through process
  $poser->work_through_pose_step = (int)$_POST['resume'];
  file_put_contents('../data/resume.json', (int)$_POST['resume']);

  if (!isset($_POST['skip'])) {
    unset($_POST['work_through']);
    unset($_POST['resume']);

    $poser->save($_POST);
  }
  else
    $poser->skip($_POST['key']);
}
//endregion Handle work_through steps, with a pose number