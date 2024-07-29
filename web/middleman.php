<?php
require '../logic/autoload.php';
$poser = new pose_organizer();

//region Handle a pose folder on the step of configuring settings
if (
  isset($_GET['configure']) ||
  isset($_GET['configure_tags']) ||
  isset($_GET['configure_review'])
) {
  require 'templates/configure.php';
}

if (isset($_POST['configure']) || isset($_POST['configure_tags'])) {
  require 'handle/configure.php';
}
//endregion Handle a pose folder on the step of configuring settings

//region Handle work_through steps, with a pose number
if (isset($_GET['find_incomplete'])) {
  exit(
  $poser->find_first_incomplete_pose()
  );
}

if (isset($_POST['work_through'], $_POST['resume'])) {
  //Update the step in the work_through process
  $poser->work_through_pose_step = (int)$_POST['resume'];
  file_put_contents('../data/resume.json', (int)$_POST['resume']);

  if (!isset($_POST['skip'])) {
    unset($_POST['work_through']);
    unset($_POST['resume']);

    try {
      $poser->save($_POST);
    } catch (Exception $e) {
      exit(
          '<div id="loading">Failed to Parse (#resume)</div>' .
          '<script>body.find("#loading").show();console.debug("' . $e->getMessage() . '")</script>'
      );
    }
  }
  else {
    $poser->skip($_POST['key']);
  }
}

else if (isset($_GET['work_through'])) {
    //Set up the file to hold the step in the work_through process
  file_put_contents('../data/resume.json', $_GET['resume'] ?? 0);
  try {
    $poser->parse_folder();
    require 'templates/work_through.php';
  } catch (Exception $e) {
    exit(
        '<div id="loading">Failed to Parse (#setup)</div>' .
        '<script>body.find("#loading").show();console.debug("' . $e->getMessage() . '")</script>'
    );
  }

}
//endregion Handle work_through steps, with a pose number
