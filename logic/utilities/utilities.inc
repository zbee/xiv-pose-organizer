<?php

class utilities {

  public static function rrmdir($dir) {
    if (is_dir($dir)) {
      $objects = scandir($dir);
      foreach ($objects as $object) {
        if ($object != "." && $object != "..") {
          if (is_dir($dir. DIRECTORY_SEPARATOR .$object) && !is_link($dir."/".$object))
            rrmdir($dir. DIRECTORY_SEPARATOR .$object);
          else
            unlink($dir. DIRECTORY_SEPARATOR .$object);
        }
      }
      rmdir($dir);
    }
  }

  //region Tag Functions
  public static function add_tag(): string {
    return '<a class="add_tag"><span style="font-weight:900">&plus;</span> Add</a>';
  }

  public static function remove_button(): string {
    return '<a style="font-weight:900;color:#E53935" ' .
      'onclick="$(this).parent().parent().remove()">&#10005;</a>';
  }

  public static function tag($name, $have_remove = true): string {
    return '<span style="display:block" class="tag">' .
      $name .
      ' <span>' .
      ($have_remove ? self::remove_button() : '') .
      '</span></span>';
  }

  //endregion Tag Functions

}