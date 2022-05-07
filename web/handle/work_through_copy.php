<?php
require '../../logic/autoload.php';
$poser = new pose_organizer('../../', prevent_checksum: true);

if (isset($_POST['copy_type'])) {
  $pose = $poser->copy_pose_fields($_POST['copy_type']);

  if ($pose->name === 'empty') {
    exit('');
  }

  try {
    echo json_encode($pose, JSON_THROW_ON_ERROR);
  } catch (JsonException $e) {
    exit('');
  }
}