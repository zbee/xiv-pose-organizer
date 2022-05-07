<?php
/**
 * @var pose_organizer $poser
 * @noinspection PhpRedundantVariableDocTypeInspection
 */
$pose = $poser->current_pose();

// Try to stop the current_pose from jumping around
file_put_contents('../data/resume.json', $poser->current_pose(false, true));

// Format the pose path data
$path = str_replace($poser->poses_folder, '', $pose->path);
$path = explode('\\', $path);
$file = $path[count($path) - 1];
/** @noinspection PhpInvalidStringOffsetUsageInspection */
unset($path[count($path) - 1]);
$path = implode("<wbr>\\", $path);

// Format the pose image data
$image          = '';
$original_image = $poser->poses_folder . $pose->image_name;
$tmp_image      = '..\\web\\' . $pose->image_name;
if ($pose->has_preview && (is_file($original_image) || is_file($tmp_image))) {
  $extension = explode('.', $pose->image_name);
  $extension = $extension[count($extension) - 1];
  $image     = "assets\\tmp\\$poser->work_through_pose_step.$extension";
  if (!str_contains($pose->image_name, 'assets\\tmp')) {
    copy($original_image, $image);
  }
  else {
    copy($tmp_image, $image);
  }
}
else {
  $pose->has_preview = false;
  $pose->image_name  = null;
}

// Hide pack-dependent elements
if (!$pose->pack) {
  echo '<style>.pack-dependent{display:none}</style>';
}

// Pass on variably required fields
$script = '<script>required_data = {';
$script .= $poser->want_pose_previews ? 'preview: true,' : 'preview: false,';
$script .= $poser->want_pose_link ? 'link: true,' : 'link: false,';
echo $script . '}</script>';
?>

<div style="display:none" id="loading">
  <span id="loader">&#9187;</span>
</div>

<div id="work_through">
  <div id="progress">
    <div>
      <div style="width:<?= $poser->progress_bar_percentage() ?>%" id="bar">
        <?= $poser->progress_bar_percentage() ?>% -
        <?= $poser->pose_count() - $poser->work_through_pose_step ?> left
      </div>
    </div>
  </div>

  <div id="info">
    <input name="key" value="<?= $pose->key ?>" type="hidden">

    <p>File Name</p>
    <div class="focus">
      <span onclick="copy_to_clipboard($(this))"
            tabindex="10"><sub
        ><?= $poser->poses_folder ?><?= $path ?>\</sub><?= $file ?></span>
    </div>

    <p>Path to File</p>
    <div class="focus">
      <span onclick="copy_to_clipboard($(this))"
            tabindex="20"><sub><?= $poser->poses_folder ?></sub><?= $path ?></span>
    </div>

    <div id="image">
      <?= $pose->has_preview ? "<img src='$image' alt='Pose preview image'>
          <input name='has_preview' value='1' type='hidden'>
          <input name='image_name' value='$image' type='hidden'>" : '<div style="border: rgba(0, 0, 0, 0.1) solid 4px;padding:10px;
          border-radius:10px">
           
        <h2>Ctrl+V</h2> to paste preview image (<code>WIN+Shift+S</code>)
        </div>
        <input name="has_preview" value="0" type="hidden">' ?>
    </div>
    <form id="upload_preview_image" style="display:none">
      <input name="file" type="file" id="preview_image_input"/>
    </form>

    <?php if (
      $poser->normalize_poses and
      $pose->file_format != $poser->format_for_pose_normalization
    ): ?>
      <p>File Format</p>
      <span style="background: var(--nord11); color: var(--nord0)">
        <?= ucfirst(constant('file_format_' . $pose->file_format)) ?>
        (needs changed)
      </span>
    <?php elseif (
      $poser->normalize_poses and
      $pose->file_format == $poser->format_for_pose_normalization
    ): ?>
      <p>File Format</p>
      <span style="background: var(--nord14); color: var(--nord0)">
        <?= ucfirst(constant('file_format_' . $pose->file_format)) ?>
      </span>
    <?php endif; ?>

    <p>In a pack?</p>
    <div style="display: inline-block;max-width: 92%">
      <label style="display: inline-block; margin-right: 5px"
             class="focus">
        <input type="radio" value="1" name="pack" id="pack_yes"
          <?= $pose->pack ? 'checked' : '' ?> tabindex="30"/>
        Yes
      </label>
      <label style="display: inline-block; margin-left: 5px" class="focus">
        <input type="radio" value="0" name="pack" id="pack_no"
          <?= $pose->pack ? '' : 'checked' ?> tabindex="35"/>
        No
      </label>
    </div>

    <label for="pack_names" class="pack-dependent">Pack Name</label>
    <div class="pack-dependent">
      <select name="pack_names" id="pack_names" tabindex="36" multiple>
        <?php foreach ($poser->get_packs() as $key => $pack): ?>
          <option value="<?= $key ?>"
            <?= $pose->pack_name == $key ? 'selected' : '' ?>
          ><?= $pack ?></option>
        <?php endforeach; ?>
      </select>
      <label style="margin-top:2px">
        <input type="text" tabindex="37" style="width: 75%"/>
        <a class="add_category" tabindex="38">
          Add
        </a>
      </label>
    </div>
    <div class="pack-dependent"></div>
    <div style="text-align:right;position:relative;top:-15px"
         class="pack-dependent focus">
      <a href="" target="_blank" id="xivma_pack_search" tabindex="38"
         data-link="https://www.xivmodarchive.com/search?types=14%2C11%2C5%2C13%2C6&basic_text=">
        Search on XIVMod
      </a>
    </div>
  </div>

  <div id="data">
    <label for="name">Pose Name</label>
    <div class="focus">
      <input type="text" name="name" id="name" value="<?= $pose->name ?>"
             tabindex="40">
    </div>

    <div></div>
    <div style="text-align:right;position:relative;top:-15px" class="focus">
      <a href="" target="_blank" tabindex="50" id="xivma_search"
         data-link="https://www.xivmodarchive.com/search?types=14%2C11%2C5%2C13%2C6&basic_text=">
        Search on XIVMod
      </a>
    </div>

    <label for="link">Pose Link</label>
    <div class="focus">
      <input type="text" name="link" id="link" value="<?= $pose->link ?>"
             tabindex="60">
    </div>

    <label for="author">Pose Author</label>
    <div class="focus">
      <input type="text" name="author" id="author" value="<?= $pose->author ?>"
             tabindex="70">
    </div>

    <div></div>
    <div style="text-align:right;position:relative;top:-15px" class="focus">
      <a href="" target="_blank" tabindex="71" id="xivma_author_search"
         data-link="https://www.xivmodarchive.com/search?types=14%2C11%2C5%2C13%2C6&basic_text=">
        Search on XIVMod
      </a>
    </div>

    <div></div>
    <div id="author_fill">&nbsp;</div>

    <label for="gender">Gender</label>
    <label for="race">Race</label>
    <select multiple name="gender" id="gender" tabindex="80">
      <?php foreach (genders as $gender): ?>
        <option value="<?= (int)constant('gender_' . $gender) ?>"
          <?= $pose->gender === constant('gender_' . $gender) ? 'selected'
            : '' ?>><?= $gender ?></option>
      <?php endforeach; ?>
    </select>
    <select multiple name="race" id="race" tabindex="90">
      <?php foreach (races as $race): ?>
        <option value="<?= $race ?>" <?= $pose->race == $race ? 'selected'
          : '' ?>><?= constant('race_' . $race)[0] ?></option>
      <?php endforeach; ?>
    </select>

    <div class="pack-dependent"></div>
    <div class="pack-dependent"></div>

    <div style="grid-column: 1/-1; display: grid;
      grid-template-columns: 1fr 1fr 1fr 1fr">
      <span style="grid-column: 1/-1;margin-bottom: 7px;"
            class="pack-dependent">
        Required=minimum # for the pose pack to work.
        Posed=maximum #
      </span>
      <label for="other_people_required" class="pack-dependent">
        Others Required
      </label>
      <div style="white-space: nowrap;height: 30px">
        <input type="number" name="other_people_required"
               id="other_people_required" class="pack-dependent"
               value="<?= $pose->other_people_required ?>"
               style="width:70%;padding:0 10px" min="0" max="15" tabindex="100">
      </div>
      <label for="other_people_posed" class="pack-dependent">
        Others Posed
      </label>
      <div style="white-space: nowrap;height: 30px">
        <input type="number" name="other_people_posed" id="other_people_posed"
               value="<?= $pose->other_people_posed ?>"
               style="width:70%;padding:0 10px" min="0" max="15"
               class="pack-dependent" tabindex="110">
      </div>
    </div>

    <label for="submission_to_other" class="pack-dependent">
      Role in group
    </label>
    <select multiple name="submission_to_other" id="submission_to_other"
            style="height: 75px" tabindex="120"
            class="pack-dependent">
      <option value=""
        <?= $pose->submission_to_other === null && $pose->worked_through
          ? 'selected' : '' ?>>
        Equal
      </option>
      <option value="1"
        <?= $pose->submission_to_other === true ? 'selected' : '' ?>>
        Submissive in group pose
      </option>
      <option value="0"
        <?= $pose->submission_to_other === false ? 'selected' : '' ?>>
        Dominant in group pose
      </option>
    </select>
  </div>

  <div id="tags">
    <h3>Categories</h3>
    <div class="column_holder">
      <?php
      $count = 0;
      foreach ($poser->get_categories() as $index => $categories):
        $count++;
        ?>
        <div>
          <label for="categories<?= $index ?>" style="display:none">
            Choose category <?= $index ?>
          </label>
          <select name="categories<?= $index ?>" id="categories<?= $index ?>"
                  data-column="<?= $index ?>" class="categories"
                  tabindex="<?= 170 + $index ?>" multiple>
            <?php foreach ($categories as $category): ?>
              <option value="<?= $category ?>"
                <?= ($pose->categories != null and
                  in_array($category, $pose->categories)) ? 'selected' : '' ?>
              ><?= $category ?></option>
            <?php endforeach; ?>
          </select>
          <label>
            <input type="text" tabindex="<?= 200 + ($index + 1) * 10 ?>"/>
            <a class="add_category" tabindex="<?= 201 + ($index + 1) * 10 ?>"
               onclick="add_category($(this))">
              Add
            </a>
          </label>
        </div>
      <?php endforeach; ?>
      <?php for ($i = $count; $i <= 2; $i++): ?>
        <div>
          <label>
            <a class="add_column" data-index="<?= $i + 1 ?>"
               data-type="categories" data-tab="170" tabindex="<?= 250 + $i ?>"
               onclick="add_column($(this))">
              <span style="font-weight:900">&plus;</span>
              Add Category Column
            </a>
          </label>
        </div>
      <?php endfor; ?>
    </div>
    <h3>Tags</h3>
    <div class="column_holder">
      <?php
      $count = 0;
      foreach ($poser->tags as $index => $tags):
        $count++;
        ?>
        <div>
          <label for="tags<?= $index ?>" style="display:none">
            Choose tags <?= $index ?>
          </label>
          <select name="tags<?= $index ?>" id="tags<?= $index ?>"
                  tabindex="<?= 260 + $index ?>"
                  data-column="<?= $index ?>" class="tags" multiple>
            <?php foreach ($tags as $tag): ?>
              <option value="<?= $tag ?>"
                <?= ($pose->categories !== null &&
                  in_array($tag, $pose->tags, true)) ? 'selected' : '' ?>
              ><?= $tag ?></option>
            <?php endforeach; ?>
          </select>
          <label>
            <input type="text" tabindex="<?= 290 + ($index + 1) * 10 ?>"/>
            <a class="add_category" tabindex="<?= 291 + ($index + 1) * 10 ?>"
               onclick="add_category($(this))">
              Add
            </a>
          </label>
        </div>
      <?php endforeach; ?>
      <?php for ($i = $count; $i <= 2; $i++): ?>
        <div>
          <label>
            <a class="add_column" data-index="<?= $i + 1 ?>" data-type="tags"
               tabindex="<?= 350 + $i ?>" onclick="add_column($(this))"
               data-tab="260">
              <span style="font-weight:900">&plus;</span>
              Add Tag Column
            </a>
          </label>
        </div>
      <?php endfor; ?>
    </div>
    <h3>Action</h3>
    <div class="column_holder">
      <div>
        <label for="verbs" style="display:none">
          Choose action
        </label>
        <select name="verbs" id="verbs" tabindex="360" multiple>
          <?php foreach ($poser->verbs as $verb): ?>
            <option value="<?= $verb ?>"
              <?= ($pose->verb !== null && $verb === $pose->verb) ? 'selected'
                : '' ?>
            ><?= $verb ?></option>
          <?php endforeach; ?>
        </select>
        <label>
          <input type="text" tabindex="370"/>
          <a class="add_category" tabindex="380"
             onclick="add_category($(this))">
            Add
          </a>
        </label>
      </div>
      <div style="text-align: center" class="focus-nav">
        <div class="navigate" style="position:static;font-size:80px"
             data-find_incomplete="" tabindex="390" onclick="navigate($(this))">
          &#9858;
        </div>
        <h1 style="margin-top:0">Unedited</h1>
      </div>
      <div style="text-align: center" class="focus-nav">
        <div class="navigate" style="position:static;font-size:80px"
             data-current="work_through" data-target="work_through"
             data-resume="<?= $poser->work_through_pose_step + 1 ?>"
             data-skip="" tabindex="400" onclick="navigate($(this))">
          &#11122;
        </div>
        <h1 style="margin-top:0">Skip</h1>
      </div>
    </div>
  </div>

  <nav class="navigate" data-current="work_through"
       data-target="<?= $poser->work_through_pose_step < 1 ? 'configure_review'
         : 'work_through' ?>" tabindex="1" onclick="navigate($(this))"
       data-resume="<?= $poser->work_through_pose_step < 1 ? ''
         : $poser->work_through_pose_step - 1 ?>">
    &laquo;
  </nav>
  <nav class="navigate" data-current="work_through"
       data-target="work_through" tabindex="2" onclick="navigate($(this))"
       data-resume="<?= $poser->work_through_pose_step ?>" data-reset="">
    &#11119;
  </nav>
  <nav class="navigate" data-current="work_through"
       data-target="<?= $poser->work_through_pose_step == $poser->pose_count()
         ? 'review' : 'work_through' ?>" tabindex="410"
       data-resume="<?= $poser->work_through_pose_step == $poser->pose_count()
         ? 'last' : $poser->work_through_pose_step + 1 ?>"
       onclick="navigate($(this))">
    &raquo;
  </nav>
</div>

<script>
  update_search_link('xivma_search', 'input[name="name"]');
  update_search_link('xivma_pack_search', 'select[name="pack_names"]');

  if (typeof fileInput === 'undefined') {
    // Handle pasting of images
    var form = document.getElementById("upload_preview_image");
    var fileInput = document.getElementById("preview_image_input");
    window.addEventListener('paste', e => {
      if (e.clipboardData.files.length < 1)
        return;

      fileInput.files = e.clipboardData.files;
      $.ajax({
        url: "/handle/work_through_image.php",
        type: "POST",
        data: new FormData(form),
        processData: false,
        cache: false,
        contentType: false,
        success: function (data) {
          if (data === 'invalid') {
            $("#err").html("Invalid File !").fadeIn();
          } else {
            $("#image").html(data).fadeIn();
            $("#upload_preview_image")[0].reset();
          }
        }
      });
    });
  }
</script>
