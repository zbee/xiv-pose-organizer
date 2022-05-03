<?php
require '../../logic/autoload.php';
$poser = new pose_organizer('../../');

$valid_extensions = ['jpeg', 'jpg', 'png', 'gif'];
$path             = '..\\assets\\tmp\\'; // upload directory

$img = $_FILES['file']['name'];
$tmp = $_FILES['file']['tmp_name'];
// get uploaded file's extension
$ext = strtolower(pathinfo($img, PATHINFO_EXTENSION));
// can upload same image using rand function
$final_image = $poser->work_through_pose_step . '.' . $ext;
// check's valid format
if (in_array($ext, $valid_extensions)) {
  $path = $path . strtolower($final_image);
  $time = time();
  $move = move_uploaded_file($tmp, $path);
  $path = str_replace('..\\', '', $path);
  if ($move) {
    echo "<img src='$path?$time' alt='Pose preview image' />
          <input name='has_preview' value='1' type='hidden'>
          <input name='image_name' value='$path' type='hidden'>";
  }
}
else {
  echo 'invalid';
}