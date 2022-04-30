<?php
if (isset($_POST['configure'])) {
  if (!file_exists('../data/settings.json')) {
    $settings_file = fopen('../data/settings.json', 'w');
    fclose($settings_file);
    $settings = $_POST;
  } else {
    $settings = (array)json_decode(file_get_contents('../data/settings.json'));
    $settings = array_merge($settings, $_POST);
  }

  if ($settings['poses_folder'] === null || $settings['poses_folder'] === '')
    $settings['poses_folder'] =
      getenv("HOMEDRIVE") .
      getenv("HOMEPATH") .
      '\Documents\Anamnesis\Poses';

  unset($_POST['configure']);

  file_put_contents(
    '../data/settings.json',
    json_encode($settings)
  );
}

if (isset($_POST['configure_tags'])) {
  $settings = (array)json_decode(file_get_contents('../data/settings.json'));
  unset($_POST['configure_tags']);
  $settings = array_merge($settings, $_POST);

  file_put_contents(
    '../data/settings.json',
    json_encode($settings)
  );
}
