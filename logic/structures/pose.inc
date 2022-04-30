<?php

class pose {
  /**
   * @var string $name Pose's assigned name
   */
  public string $name;
  /**
   * @var string $path Pose's location
   */
  public string $path;
  /**
   * @var null|string $author Pose's creator
   */
  public ?string $author = null;
  /**
   * @var null|string $link link to the pose
   */
  public ?string $link = null;
  /**
   * @var bool $has_preview whether the pose has a preview image in place
   */
  public bool $has_preview = false;
  /**
   * @var null|string $image_name the name of the image file
   */
  public ?string $image_name = null;
  /**
   * @var null|string[] $categories an array of category names
   */
  public ?array $categories;
  /**
   * @var null|string[] $tags an array of tag names
   */
  public ?array $tags;
  /**
   * @var null|string $race name of race, see constants.inc
   */
  public ?string $race = null;
  /**
   * @var null|bool $gender a boolean of the gender, or none specified
   *                        true: male
   *                        false: female
   */
  public ?bool $gender = null;
  /**
   * @var int $other_people_posed the quantity of other people **included**
   */
  public int $other_people_posed = 0;
  /**
   * @var int $other_people_required the quantity of other people **required**
   */
  public int $other_people_required = 0;
  /**
   * @var null|bool $submission_to_other if the pose could be considered either,
   *                                     whether it is sub or dom in the pose.
   *                                     Assumes $other_people_required > 0;
   *                                     Null if that's not the case.
   */
  public ?bool $submission_to_other = null;
  /**
   * @var null|string $verb the chosen verb tag
   */
  public ?string $verb = null;
  /**
   * @var bool $worked_through whether this pose has already been processed
   */
  public bool $worked_through = false;
  /**
   * @var bool $skipped whether this pose was skipped when worked through
   */
  public bool $skipped = false;
  /**
   * @var string $file_format the 3-letter identifier of the pose's file format
   */
  public string $file_format = '';
  /**
   * @var bool $formatted whether the pose has been formatted to config
   */
  public bool $formatted = false;
  /**
   * @var bool $checked whether the pose has been checked
   */
  public bool $checked = false;
  /**
   * @var bool $normalized whether pose's file format has been normalized
   *                       null if not required
   */
  public ?bool $normalized = null;

  /**
   * @var string $key hashed pose file data
   */
  public string $key;

  /**
   * @var null|string $duplicate_pose if not null then there is a pose of same
   *                                  file name but different extension and will
   *                                  be a string of that duplicate's path
   *                                  This attribute will be on the pose struct
   *                                  that is of the desired file format
   */
  public ?string $duplicate_pose = null;

  /**
   * @var bool $pack true if this pose is likely part of a pack
   */
  public bool $pack = false;

  /**
   * @var null|string $pack_name they key of the pack this pose is part of
   */
  public ?string $pack_name = null;

  public function __construct(
    mixed  $name,
    string $key = '',
    string $file_format = '',
    string $path = '',
    bool   $pack = false,
    string $pack_name = null,
    array  $categories = null,
    array  $tags = null,
    string $verb = null,
    string $author = null,
    string $link = null,
    string $image_name = null,
    string $race = null,
    bool   $gender = null,
    int    $other_people_posed = 0,
    int    $other_people_required = 0,
    bool   $submission_to_other = null,
    bool   $worked_through = false,
    bool   $skipped = false,
    bool   $formatted = false,
    bool   $checked = false,
    bool   $normalized = null,
    string $duplicate_pose = null,
  ) {
    if (gettype($name) != 'string') {
      $this->name                  = $name->name;
      $this->key                   = $name->key;
      $this->path                  = $name->path;
      $this->pack                  = $name->pack;
      $this->pack_name             = $name->pack_name;
      $this->categories            = $name->categories;
      $this->tags                  = $name->tags;
      $this->verb                  = $name->verb;
      $this->file_format           = $name->file_format;
      $this->author                = $name->author;
      $this->link                  = $name->link;
      $this->has_preview           = $name->image_name !== null;
      $this->image_name            = $name->image_name;
      $this->race                  = $name->race;
      $this->gender                = $name->gender;
      $this->other_people_posed    = $name->other_people_posed;
      $this->other_people_required = $name->other_people_required;
      $this->submission_to_other   = $name->submission_to_other;
      $this->worked_through        = $name->worked_through;
      $this->skipped               = $name->skipped;
      $this->formatted             = $name->formatted;
      $this->checked               = $name->checked;
      $this->normalized            = $name->normalized;
      $this->duplicate_pose        = $name->duplicate_pose;
    }
    else {
      $this->name                  = (string)$name;
      $this->key                   = $key;
      $this->path                  = $path;
      $this->pack                  = $pack;
      $this->pack_name             = $pack_name;
      $this->categories            = $categories;
      $this->tags                  = $tags;
      $this->verb                  = $verb;
      $this->file_format           = $file_format;
      $this->author                = $author;
      $this->link                  = $link;
      $this->has_preview           = $image_name !== null;
      $this->image_name            = $image_name;
      $this->race                  = $race;
      $this->gender                = $gender;
      $this->other_people_posed    = $other_people_posed;
      $this->other_people_required = $other_people_required;
      $this->submission_to_other   = $submission_to_other;
      $this->worked_through        = $worked_through;
      $this->skipped               = $skipped;
      $this->formatted             = $formatted;
      $this->checked               = $checked;
      $this->normalized            = $normalized;
      $this->duplicate_pose        = $duplicate_pose;
    }
  }

  /**
   * Actually applies the edits to the file and folder
   *
   * @param array $posted_data      the $_POST data from a work_through
   *                                navigation alternatively for skipping a
   *                                pose: a value of true
   */
  public function save(array $posted_data): void {
    foreach ($posted_data as $key => $value)
      $this->$key = $value;
    $this->worked_through = true;
    $this->skipped        = false;
  }

  public function skip(): void {
    $this->skipped = true;
  }

}