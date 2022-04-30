<?php
require '../../logic/autoload.php';
$poser = new pose_organizer('../../');

if (isset($_POST['key']))
  foreach ($poser->poses as $pose)
    if ($pose->pack_name == $_POST['key'])
      exit(
        json_encode(
          [
            'categories' => $pose->categories,
            'author' => $pose->author,
            'link' => $pose->link,
            'others_posed' => $pose->other_people_posed,
          ]
        )
      );