<?php
require '../../logic/autoload.php';
$poser = new pose_organizer('../../');

foreach ($poser->poses as $pose) {
  if (
    $pose->author !== null && str_starts_with($pose->author, $_POST['author'])
  ) {
    exit($pose->author);
  }
}