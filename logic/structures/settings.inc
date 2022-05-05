<?php

class settings extends utilities {

  /**
   * @var bool $use_nested_structure_for_poses
   *           If poses should be placed into a folder, or loose with same-named
   *                                        supporting files next to it
   *           default: true;
   */
  public bool $use_nested_structure_for_poses;

  /**
   * @var bool $file_and_folder_separate_tagging
   *           if the tagging should be separated between the folder name for a
   *           pose and the file name for a pose.
   *           default: true;
   *           requires: use_flat_structure_for_poses = false
   */
  public bool $file_and_folder_separate_tagging;

  /**
   * @var bool $always_attribute_to_author
   *           if the author should be attributed both in the folder and file
   *           name.
   *           default: true;
   *           requires: use_flat_structure_for_poses = false
   */
  public bool $always_attribute_to_author;

  /**
   * @var bool $normalize_poses
   *           whether nor not poses should be normalized to a specific file
   *           format.
   *           (not done automatically);
   *           default: true
   */
  public bool $normalize_poses;

  /**
   * @var string format_for_pose_normalization
   *           desired format for poses.
   *           default: 'ana';
   *           options: 'ana', 'cmt';
   *           requires: normalize_poses = true
   */
  public string $format_for_pose_normalization;

  /**
   * @var bool $keep_other_pose_formats
   *           if normalize_poses=true then whether other pose formats should
   *           be kept.
   *           default: true
   */
  public bool $keep_other_pose_formats;

  /**
   * @var bool $want_pose_previews
   *           if preview images for poses is desired next to the pose
   *           default: true
   */
  public bool $want_pose_previews;

  /**
   * @var bool $want_pose_link
   *           if links to mods are desired next to the pose
   *           default: true
   */
  public bool $want_pose_link;

  /**
   * @var bool $start_with_default_tags
   *           if tags should start with default options
   *           default: true
   */
  public bool $start_with_default_tags;

  /**
   * @var string $folder_name_format
   *             the format to formulate folder names to.
   *             format: %category_x% is iterative to however many category
   *             sections you have, and shows the one tag selected from the
   *             category
   *                     %pose_name% is the name of the pose or pose pack
   *                     %author% has logic controlling whether it's shown
   *             default: %category_1% %category_2% %category_3% %tags% %group%
   *                      %author% '%pose_name%'
   *             requires: use_flat_structure_for_poses = false
   */
  protected string $folder_name_format = "%category_1% %category_2% %category_3% " .
  "%tags% %group% %author% '%pose_name%'";

  /**
   * @var string $author_format
   *             the format for the authors name
   *             format: %author% is the name of the author
   *             default: [%author%]
   */
  public string $author_format = "[%author%]";

  /**
   * @var string file_name_format
   *             the format to formulate folder names to.
   *             format: %tag_x% is iterative to however many tag sections you
   *             have, and shows the one tag selected from each section
   *                     %pose_name% is simply the name of the pose
   *                     %author% has logic controlling whether it's shown
   *                     %verb% is implemented separately from tags, and also
   *                     shows just the one selected verb
   *                     %race% is implemented separately from tags
   *                     %gender% is implemented separately from tags
   *                     %categories% are abbreviated categories that are
   *                     default shown on folder names
   *                     requires: separate_tagging_between_file_and_folder =
   *                     true default: %categories% %tag_1% %tag_2% %tag_3%
   *                     %verb%
   *                              %sub% %race% %gender% %author% '%pose_name%'
   */
  protected string $file_name_format = "%categories% %tag_1% %tag_2% %tag_3% " .
  "%verb% %sub% %race% %gender% %author% '%pose_name%'";

  /**
   * @var string $category_format
   *             the format for the list of category names
   *             format: %category_x% the abbreviation of the category
   *                     %category_full_x% the full name of the category
   *             default: {%category_1%%category_2%%category_3%}
   */
  public string $category_format = '{%category_1%%category_2%%category_3%}';

  /**
   * @var string $poses_folder
   *                    the folder you keep your poses in
   *                    default: $_SERVER['USERPROFILE']
   *                    /Documents/Anamnesis/Poses
   */
  public string $poses_folder;

  public function __construct(
    string $poses_folder = null,
    bool   $always_attribute_to_author = true,
    bool   $keep_other_pose_formats = true,
    bool   $normalize_poses = true,
    string $format_for_pose_normalization = 'ana',
    bool   $file_and_folder_separate_tagging = true,
    bool   $start_with_default_tags = true,
    bool   $use_nested_structure_for_poses = false,
    bool   $want_pose_link = true,
    bool   $want_pose_previews = true,
    string $author_format = '[%author%]',
    string $category_format = '{%category_1%%category_2%%category_3%}',
    string $file_name_format = "%categories% %tag_1% %tag_2% %tag_3% " .
    "%verb% %sub% %race% %gender% %author% '%pose_name%'",
    string $folder_name_format = "%category_1% %category_2% %category_3% " .
    "%tags% %group% %author% '%pose_name%'",
  ) {
    if (file_exists('../data/settings.json')) {
      $settings = json_decode(
        file_get_contents('../data/settings.json'),
        true,
        512,
        JSON_THROW_ON_ERROR
      );
      extract($settings, EXTR_OVERWRITE);

      if ($poses_folder === null || $poses_folder === '') {
        $poses_folder =
          getenv("HOMEDRIVE") .
          getenv("HOMEPATH") .
          '\Documents\Anamnesis\Poses';
      }
    }
    $this->always_attribute_to_author       = $always_attribute_to_author;
    $this->author_format                    = $author_format;
    $this->category_format                  = $category_format;
    $this->file_name_format                 = $file_name_format;
    $this->folder_name_format               = $folder_name_format;
    $this->format_for_pose_normalization    = $format_for_pose_normalization;
    $this->keep_other_pose_formats          = $keep_other_pose_formats;
    $this->normalize_poses                  = $normalize_poses;
    $this->file_and_folder_separate_tagging =
      $file_and_folder_separate_tagging == 'true';
    $this->start_with_default_tags          = $start_with_default_tags;
    $this->use_nested_structure_for_poses   =
      $use_nested_structure_for_poses == 'true';
    $this->want_pose_link                   = $want_pose_link == 'true';
    $this->want_pose_previews               = $want_pose_previews == 'true';
    $this->poses_folder                     =
      $poses_folder
      ??
      getenv("HOMEDRIVE") . getenv("HOMEPATH") . '\Documents\Anamnesis\Poses';
  }
}