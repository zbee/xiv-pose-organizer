<?php
require '../../logic/autoload.php';
$poser = new pose_organizer('../../');

if (isset($_POST['key']))
  foreach ($poser->poses as $pose)
    if ($pose->pack_name == $_POST['key']) {
      $time  = time();
      $image = "<img src='$pose->image_name?$time' alt='Pose preview image' />
          <input name='has_preview' value='1' type='hidden'>
          <input name='image_name' value='$pose->image_name' type='hidden'>";

      exit(
      json_encode(
        [
          'categories'   => $pose->categories,
          'author'       => $pose->author,
          'link'         => $pose->link,
          'others_posed' => $pose->other_people_posed,
          'image'        => $image,
        ]
      )
      );
    }