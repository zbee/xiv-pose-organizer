<?php
/** @var pose_organizer $poser */
$settings = [];
if (file_exists('../data/settings.json')) {
  $settings = file_get_contents('../data/settings.json');
  echo '<script>load_settings(' . $settings . ')</script>';
}
?>

<?php if (isset($_GET['configure'])): ?>
  <div class="tip">
    These settings are not final; you will have an opportunity to
    review them before you begin working through your poses
  </div>
  <div id="configure">
    <div class="navigate"></div>
    <div id="body">
      <h1>Choose your Settings</h1>
      Load recommended settings:<br>
      <a class="load_lazy">"Just get to the tagging"</a>,
      <a class="load_tryhard">"I've got time to spare"</a>
      <br><br>
      <label id="config-pose_folder">
        Full path to your poses folder
        <input type="text" placeholder="C:\Users\Greg\Desktop\Poses"
               style="display: block;width: 275px;margin:auto">
        (Left blank this will default to: `&lt;your user&gt;<i>\Documents\Anamnesis\Poses</i>`)
      </label>
      <br><br>
      <label id="config-use_folders"
             onclick="if( $(this).find('input').prop('checked') === false ) {
           $('#config-separate_tags').hide();
           $('#config-always_attribute').hide();
           $('#config-nested_struct_alternative').show();
         } else {
           $('#config-separate_tags').show();
           $('#config-always_attribute').show();
           $('#config-nested_struct_alternative').hide();
         }">
        <input type="checkbox" data-key="use_nested_structure_for_poses" checked>
        Put poses into their own folder
        <br><i>(In the Review Step you will be able to select multiple poses to
          put into
          the same folder)</i>
      </label>
      <p id="config-nested_struct_alternative"
         style="font-size:12px;display:none;">
        If this is disabled, poses will just be kept loosely, with preview
        images, links to author / mod, and original pose file format right next
        to it with the same name.
        <br>
        The main benefit of this is scrolling through the preview pictures in
        your pose folder in explorer to find something you like.
      </p>
      <label id="config-separate_tags">
        <input type="checkbox" data-key="file_and_folder_separate_tagging"
               checked>
        Separate categories into folder names and tags to file names
      </label>
      <label id="config-always_attribute">
        <input type="checkbox" data-key="always_attribute_to_author" checked>
        Attribute pose author in folder <b>and</b> file name
      </label>
      <br>
      <hr>
      <br>
      <label id="config-normalize_files"
             onclick="if( $(this).find('input').prop('checked') === false ) {
           $('#config-normalize_format').hide();
           $('#config-normalize_keep_other').hide();
         } else {
           $('#config-normalize_format').show();
           $('#config-normalize_keep_other').show();
         }">
        <input type="checkbox" data-key="normalize_poses" checked>
        Have the Review Step remind you to normalize pose files to same format
      </label>
      <label id="config-normalize_format">
        File format to normalize to:
        <select>
          <option value="ana" selected>Anamnesis</option>
          <option value="cmt">Concept Matrix</option>
        </select>
      </label>
      <br>
      <label id="config-normalize_keep_other">
        <input type="checkbox" data-key="keep_other_pose_formats" checked>
        Keep pose files of the other format after the pose is marked as
        normalized to the desired format
      </label>
      <br>
      <hr>
      <br>
      <label id="config-preview_desired">
        <input type="checkbox" data-key="want_pose_previews" checked>
        Have the Review Step remind you to create preview images
        <br>
        <b>AND</b> keep the preview images that currently exist next to the pose
        files
      </label>
      <br>
      <label id="config-link_desired">
        <input type="checkbox" data-key="want_pose_link" checked>
        Have the Pose Saving Step create a web link shortcut file next to the
        pose file
        <br>
        <b>AND</b> have the Review Step remind you to find links to poses
      </label>
    </div>
    <div class="navigate" data-current="configure" data-target="configure_tags">
      &raquo;
    </div>
  </div>
<?php endif; ?>

<?php if (isset($_GET['configure_tags'])): ?>
  <div class="tip">
    These settings are not final; you will have an opportunity to
    review them before you begin working through your poses
  </div>
  <div id="configure" style="width:1200px">
    <div class="navigate" data-current="configure_tags" data-target="configure">
      &laquo;
    </div>
    <div id="body">
      <h1>Set up some Tags to start with</h1>
      For reference, you will only be able to pick one tag or category from each
      column.
      <br>
      You will be able to add additional entries to each column, and additional
      columns later on.
      <div style="display:grid;grid-template-columns:50% 50%;">
        <a class="populate_defaults" style="grid-column: 1 / span 2;">
          <span style="font-weight:900">&plus;</span>
          Populate default categories and tags
        </a>
        <div>
          <h3>Categories</h3>
          <div style="display:grid;grid-auto-columns: auto;">
            <div id="categories" class="column_holder"
                 style="grid-row:1;display:grid;grid-auto-columns: auto;">
              <?php //region Fill in categories from settings ?>
              <?php if (file_exists('../data/settings.json')): ?>
                <?php $settings = json_decode(
                  file_get_contents('../data/settings.json'),
                  true
                ); ?>
                <?php foreach ($settings as $setting => $value): ?>
                  <?php if ($setting == 'categories'): ?>
                    <?php foreach ($value as $number => $category): ?>
                      <div style="display:inline-block;grid-row:1">
                        <h4>
                          Category <?php echo $number ?>
                          <?php echo $poser->remove_button() ?>
                        </h4>
                        <?php foreach ($category as $tag): ?>
                          <?php echo $poser->tag($tag) ?>
                        <?php endforeach ?>
                        <?php echo $poser->add_tag() ?>
                      </div>
                    <?php endforeach ?>
                  <?php endif ?>
                <?php endforeach ?>
              <?php endif ?>
              <?php //endregion Fill in categories from settings ?>
            </div>
            <div style="grid-row:1;">
              <a class="add_category">
                <h4>
                  <span style="font-weight:900">&plus;</span> Add a category
                </h4>
              </a>
            </div>
          </div>
        </div>
        <div>
          <h3>Tags</h3>
          <div style="display:grid;grid-auto-columns: auto;">
            <div id="tags" class="column_holder"
                 style="grid-row:1;display:grid;grid-auto-columns: auto;">
              <?php //region Fill in tags from settings ?>
              <?php if (file_exists('../data/settings.json')): ?>
                <?php $settings = json_decode(
                  file_get_contents('../data/settings.json'),
                  true
                ); ?>
                <?php foreach ($settings as $setting => $value): ?>
                  <?php if ($setting == 'tags'): ?>
                    <?php foreach ($value as $number => $category): ?>
                      <div style="display:inline-block;grid-row:1">
                        <h4>
                          Tag <?php echo $number ?>
                          <?php echo $poser->remove_button() ?>
                        </h4>
                        <?php foreach ($category as $tag): ?>
                          <?php echo $poser->tag($tag) ?>
                        <?php endforeach ?>
                        <?php echo $poser->add_tag() ?>
                      </div>
                    <?php endforeach ?>
                  <?php endif ?>
                <?php endforeach ?>
              <?php endif ?>
              <?php //endregion Fill in tags from settings ?>
            </div>
            <div style="grid-row:1;">
              <a class="add_category">
                <h4>
                  <span style="font-weight:900">&plus;</span> Add a tag
                </h4>
              </a>
            </div>
          </div>
        </div>
      </div>
    </div>
    <div class="navigate" data-current="configure_tags"
         data-target="configure_review">&raquo;
    </div>
  </div>
<?php endif; ?>

<?php if (isset($_GET['configure_review'])): ?>
  <div id="configure" style="width:900px">
    <div class="navigate" data-target="configure_tags">&laquo;</div>
    <div id="body">
      <h1>Check your settings</h1>
      <i>
        (Some settings that may not be relevant will be displayed here -
        if you are confused then go back 2 pages to verify)
      </i>
      <br><br>
      <label id="config-pose_folder">
        Full path to your poses folder
        <input type="text" style="display: block;width: 275px;margin:auto"
               disabled>
      </label>
      <div style="display:grid;grid-template-columns:30% 30% 30%;">
        <div>
          <label id="config-use_folders">
            <input type="checkbox" data-key="use_nested_structure_for_poses"
                   disabled>
            Put poses into their own folder
          </label>
          <p id="config-nested_struct_alternative"
             style="font-size:12px;display:none;">
            If this is disabled, poses will just be kept loosely
          </p>
          <label id="config-separate_tags">
            <input type="checkbox" data-key="file_and_folder_separate_tagging"
                   disabled>
            Separate categories into folder names and tags to file names
          </label>
          <label id="config-always_attribute">
            <input type="checkbox" data-key="always_attribute_to_author"
                   disabled>
            Attribute pose author in folder <b>and</b> file name
          </label>
        </div>
        <div>
          <label id="config-normalize_files">
            <input type="checkbox" data-key="normalize_poses" disabled>
            Remind you to normalize pose files to same format
          </label>
          <label id="config-normalize_format">
            File format to normalize to:
            <select disabled>
              <option value="ana">Anamnesis</option>
              <option value="cmt">Concept Matrix</option>
            </select>
          </label>
          <br>
          <label id="config-normalize_keep_other">
            <input type="checkbox" data-key="keep_other_pose_formats" disabled>
            Keep pose files of the other format
          </label>
        </div>
        <div>
          <label id="config-preview_desired">
            <input type="checkbox" data-key="want_pose_previews" disabled>
            Have the Review Step remind you to create preview images
          </label>
          <br>
          <label id="config-link_desired">
            <input type="checkbox" data-key="want_pose_link" disabled>
            Have the Pose Saving Step create a web link shortcut file
          </label>
        </div>
      </div>
      <div style="display:grid;grid-template-columns:50% 50%;">
        <div>
          <h3>Categories</h3>
          <div style="display:grid;grid-auto-columns: auto;">
            <div id="categories" class="column_holder"
                 style="grid-row:1;display:grid;grid-auto-columns: auto;">
              <?php //region Fill in categories from settings ?>
              <?php if (file_exists('../data/settings.json')): ?>
                <?php $settings = json_decode(
                  file_get_contents('../data/settings.json'),
                  true
                ); ?>
                <?php foreach ($settings as $setting => $value): ?>
                  <?php if ($setting == 'categories'): ?>
                    <?php foreach ($value as $number => $category): ?>
                      <div style="display:inline-block;grid-row:1">
                        <h4>
                          Category <?php echo $number ?>
                        </h4>
                        <?php foreach ($category as $tag): ?>
                          <?php echo $poser->tag($tag, have_remove: false) ?>
                        <?php endforeach ?>
                      </div>
                    <?php endforeach ?>
                  <?php endif ?>
                <?php endforeach ?>
              <?php endif ?>
              <?php //endregion Fill in categories from settings ?>
            </div>
          </div>
        </div>
        <div>
          <h3>Tags</h3>
          <div style="display:grid;grid-auto-columns: auto;">
            <div id="tags" class="column_holder"
                 style="grid-row:1;display:grid;grid-auto-columns: auto;">
              <?php //region Fill in tags from settings ?>
              <?php if (file_exists('../data/settings.json')): ?>
                <?php $settings = json_decode(
                  file_get_contents('../data/settings.json'),
                  true
                ); ?>
                <?php foreach ($settings as $setting => $value): ?>
                  <?php if ($setting == 'tags'): ?>
                    <?php foreach ($value as $number => $category): ?>
                      <div style="display:inline-block;grid-row:1">
                        <h4>
                          Tag <?php echo $number ?>
                        </h4>
                        <?php foreach ($category as $tag): ?>
                          <?php echo $poser->tag($tag, have_remove: false) ?>
                        <?php endforeach ?>
                      </div>
                    <?php endforeach ?>
                  <?php endif ?>
                <?php endforeach ?>
              <?php endif ?>
              <?php //endregion Fill in tags from settings ?>
            </div>
          </div>
        </div>
      </div>
    </div>
    <div class="navigate" data-target="work_through">&raquo;</div>
  </div>
<?php endif; ?>