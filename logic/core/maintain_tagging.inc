<?php

/**
 * Simple class with getters and setters for tags and categories that also
 * maintain them in the settings.json file
 */
class maintain_tagging extends pose_folder_parser {
  /**
   * @var string[][] $tags List of categories of tags, with individual tags
   */
  public array $tags;

  /**
   * @var string[][] $categories List of categories of categories, with
   * individual categories
   */
  public array $categories;

  /**
   * @var string[] $verbs List of verbs that have been used
   */
  public array $verbs;

  /**
   * @return string[][]
   */
  public function get_verbs(): array {
    if (!isset($this->verbs)) {
      $this->set_verb_file();
      $settings  =
        json_decode(file_get_contents('../data/settings.json'), true);
      $verb_list = json_decode(file_get_contents('../data/verbs.json'), true);
      $this->set_tag_data(
        $settings['tags'],
        $settings['categories'],
        $verb_list,
        false
      );
    }
    return $this->tags;
  }

  /**
   * @return string[][]
   */
  public function get_tags(): array {
    if (!isset($this->tags)) {
      $this->set_verb_file();
      $settings  =
        json_decode(file_get_contents('../data/settings.json'), true);
      $verb_list = json_decode(file_get_contents('../data/verbs.json'), true);
      $this->set_tag_data(
        $settings['tags'],
        $settings['categories'],
        $verb_list,
        false
      );
    }
    return $this->tags;
  }

  /**
   * @return string[][]
   */
  public function get_categories(): array {
    if (!isset($this->tags)) {
      $this->set_verb_file();
      $settings  =
        json_decode(file_get_contents('../data/settings.json'), true);
      $verb_list = json_decode(file_get_contents('../data/verbs.json'), true);
      $this->set_tag_data(
        $settings['tags'],
        $settings['categories'],
        $verb_list,
        false
      );
    }
    return $this->categories;
  }

  /**
   * @param string[][] $tags
   * @param string[][] $categories
   * @param string[]   $verbs
   * @param bool       $save_data     Whether saving the categories and tags
   *                                  should update settings.json and verbs.json
   */
  public function set_tag_data(
    array $tags,
    array $categories,
    array $verbs,
    bool  $save_data = true
  ): void {
    if ($save_data) {
      // Load the current settings
      $settings = json_decode(file_get_contents('../data/settings.json'), true);

      // Swap in the new tags and categories
      $settings['tags']       = $tags;
      $settings['categories'] = $categories;

      // Save the settings
      file_put_contents('../data/settings.json', json_encode($settings));

      // Save the verb list
      file_put_contents('../data/verbs.json', json_encode($verbs));
    }

    // Save the categories and tags to their properties
    $this->tags       = $tags;
    $this->categories = $categories;
    $this->verbs      = $verbs;
  }

  /**
   * Create the verb file if it does not yet exist
   * This is here but not for the settings file as that is handled when
   * constructed
   * @return void
   */
  private function set_verb_file(): void {
    if (!file_exists('../data/verbs.json'))
      file_put_contents('../data/verbs.json', json_encode([]));
  }
}