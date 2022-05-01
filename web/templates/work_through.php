<?php
/** @var pose_organizer $poser */
$pose = $poser->current_pose();

// Try to stop the current_pose from jumping around
file_put_contents('../data/resume.json', $poser->current_pose(false, true));

// Format the pose path data
$path = str_replace($poser->poses_folder, '', $pose->path);
$path = explode('\\', $path);
$file = $path[count($path) - 1];
unset($path[count($path) - 1]);
$path = implode("<wbr>\\", $path);

// Format the pose image data
$image = '';
if ($pose->has_preview) {
  $extension = explode('.', $pose->image_name);
  $extension = $extension[count($extension) - 1];
  $image     = "assets/tmp/$poser->work_through_pose_step.$extension";

  if (!str_contains($pose->image_name, 'tmp'))
    copy($poser->poses_folder . $pose->image_name, $image);
}

// Hide pack-dependent elements
if (!$pose->pack)
  echo '<style>.pack-dependent{display:none}</style>';
?>

<div style="display:none" id="loading"><img src="/assets/img/loading.gif"></div>

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
    <span onclick="copy_to_clipboard($(this))"><sub
      ><?= $poser->poses_folder ?><?= $path ?>\</sub><?= $file ?></span>

    <p>Path to File</p>
    <span onclick="copy_to_clipboard($(this))"
    ><sub><?= $poser->poses_folder ?></sub><?= $path ?></span>

    <?php if ($poser->want_pose_previews): ?>
      <div id="image">
        <?= $pose->has_preview ? "<img src='$image' alt='Pose preview image'>
            <input name='has_preview' value='1' type='hidden'>
            <input name='image_name' value='$pose->image_name' type='hidden'>" : '<div style="border: rgba(0, 0, 0, 0.1) solid 4px;padding:10px;
            border-radius:10px">
             
          <h2>Ctrl+V</h2> to paste preview image (<code>WIN+Shift+S</code>)
          </div>
          <input name="has_preview" value="0" type="hidden">' ?>
      </div>
      <form id="upload_preview_image" style="display:none">
        <input name="file" type="file" id="preview_image_input"/>
      </form>
    <?php endif ?>

    <?php if (
      $poser->normalize_poses and
      $pose->file_format != $poser->format_for_pose_normalization
    ): ?>
      <p>File Format</p>
      <span style="background: rgba(255, 0, 0, 0.2);">
        <?= ucfirst(constant('file_format_' . $pose->file_format)) ?>
        (needs changed)
      </span>
    <?php elseif (
      $poser->normalize_poses and
      $pose->file_format == $poser->format_for_pose_normalization
    ): ?>
      <p>File Format</p>
      <span style="background: rgba(0, 255, 0, 0.2);">
        <?= ucfirst(constant('file_format_' . $pose->file_format)) ?>
      </span>
    <?php endif; ?>

    <p>In a pack?</p>
    <div style="display: inline-block;max-width: 92%">
      <label style="display: inline-block; margin-right: 5px">
        <input type="radio" value="1" name="pack" id="pack_yes"
          <?= $pose->pack ? 'checked' : '' ?>/>
        Yes
      </label>
      <label style="display: inline-block; margin-left: 5px">
        <input type="radio" value="0" name="pack" id="pack_no"
          <?= $pose->pack ? '' : 'checked' ?>/>
        No
      </label>
    </div>
  </div>

  <div id="data">
    <label for="name">Pose Name</label>
    <input type="text" name="name" id="name" value="<?= $pose->name ?>">
    <div></div>
    <div style="text-align:right;position:relative;top:-15px">
      <a href="" target="_blank"
         id="xivma_search"
         data-link="https://www.xivmodarchive.com/search?types=14%2C11%2C5%2C13%2C6&basic_text=">
        Search on XIVModArchive
      </a>
    </div>
    <?php if ($poser->want_pose_link): ?>
      <label for="link">Pose Link</label>
      <input type="text" name="link" id="link" value="<?= $pose->link ?>">
    <?php endif; ?>
    <label for="author">Pose Author</label>
    <input type="text" name="author" id="author" value="<?= $pose->author ?>">

    <div></div>
    <div></div>

    <label for="gender">Gender</label>
    <label for="race">Race</label>
    <select multiple name="gender" id="gender">
      <?php foreach (genders as $gender): ?>
        <option value="<?= (int)constant('gender_' . $gender) ?>"
          <?= $pose->gender == constant('gender_' . $gender) ? 'selected'
            : '' ?>><?= $gender ?></option>
      <?php endforeach; ?>
    </select>
    <select multiple name="race" id="race">
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
      <input type="number" name="other_people_required"
             id="other_people_required" class="pack-dependent"
             value="<?= $pose->other_people_required ?>"
             style="width:70%;padding:0 10px" min="1" max="15">
      <label for="other_people_posed" class="pack-dependent">
        Others Posed
      </label>
      <input type="number" name="other_people_posed" id="other_people_posed"
             value="<?= $pose->other_people_posed ?>"
             style="width:70%;padding:0 10px" min="1" max="15"
             class="pack-dependent">
    </div>

    <label for="submission_to_other" class="pack-dependent">
      Role in group
    </label>
    <select multiple name="submission_to_other" id="submission_to_other"
            style="height: 50px"
            class="pack-dependent">
      <option value="1" <?= $pose->submission_to_other ? 'selected' : '' ?>>
        Submissive in group pose
      </option>
      <option value="0"
        <?= $pose->submission_to_other === false ? 'selected' : '' ?>>
        Dominant in group pose
      </option>
    </select>

    <label for="pack_names" class="pack-dependent">Pack Name</label>
    <div class="pack-dependent">
      <select name="pack_names" id="pack_names" multiple>
        <?php foreach ($poser->get_packs() as $key => $pack): ?>
          <option value="<?= $key ?>"
            <?= $pose->pack_name == $key ? 'selected' : '' ?>
          ><?= $pack ?></option>
        <?php endforeach; ?>
      </select>
      <label>
        <input type="text"/>
        <a class="add_category">
          Add
        </a>
      </label>
    </div>
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
                  data-column="<?= $index ?>" class="categories" multiple>
            <?php foreach ($categories as $category): ?>
              <option value="<?= $category ?>"
                <?= ($pose->categories != null and
                  in_array($category, $pose->categories)) ? 'selected' : '' ?>
              ><?= $category ?></option>
            <?php endforeach; ?>
          </select>
          <label>
            <input type="text"/>
            <a class="add_category">
              Add
            </a>
          </label>
        </div>
      <?php endforeach; ?>
      <?php for ($i = $count; $i <= 2; $i++): ?>
        <div>
          <label>
            <a class="add_column" data-index="<?= $i + 1 ?>"
               data-type="categories">
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
                  data-column="<?= $index ?>" class="tags" multiple>
            <?php foreach ($tags as $tag): ?>
              <option value="<?= $tag ?>"
                <?= ($pose->categories != null and in_array($tag, $pose->tags))
                  ? 'selected' : '' ?>
              ><?= $tag ?></option>
            <?php endforeach; ?>
          </select>
          <label>
            <input type="text"/>
            <a class="add_category">
              Add
            </a>
          </label>
        </div>
      <?php endforeach; ?>
      <?php for ($i = $count; $i <= 2; $i++): ?>
        <div>
          <label>
            <a class="add_column" data-index="<?= $i + 1 ?>" data-type="tags">
              <span style="font-weight:900">&plus;</span>
              Add Tag Column
            </a>
          </label>
        </div>
      <?php endfor; ?>
    </div>
    <h3>Verb</h3>
    <div class="column_holder">
      <div>
        <label for="verbs" style="display:none">
          Choose verbs
        </label>
        <select name="verbs" id="verbs" multiple>
          <?php foreach ($poser->verbs as $verb): ?>
            <option value="<?= $verb ?>"
              <?= ($pose->verb != null and $verb == $pose->verb) ? 'selected'
                : '' ?>
            ><?= $verb ?></option>
          <?php endforeach; ?>
        </select>
        <label>
          <input type="text"/>
          <a class="add_category">
            Add
          </a>
        </label>
      </div>
      <div>
        <a href="?find_incomplete"
           style="color:#424242;text-decoration:none;text-align: center">
          <div class="navigate" style="position:static;font-size:80px">
            &#9858;
          </div>
          <h1 style="margin-top:0">1st Unedited</h1>
        </a>
      </div>
      <div style="text-align: center">
        <div class="navigate" style="position:static;font-size:80px"
             data-current="work_through" data-target="work_through"
             data-resume="<?= $poser->work_through_pose_step + 1 ?>"
             data-skip="">
          &#10174;
        </div>
        <h1 style="margin-top:0">Skip</h1>
      </div>
    </div>
  </div>

  <nav class="navigate" data-current="work_through"
       data-target="<?= $poser->work_through_pose_step < 1 ? 'configure_review'
         : 'work_through' ?>"
       data-resume="<?= $poser->work_through_pose_step < 1 ? ''
         : $poser->work_through_pose_step - 1 ?>">
    &laquo;
  </nav>
  <nav class="navigate" data-current="work_through"
       data-target="<?= $poser->work_through_pose_step == $poser->pose_count()
         ? 'review' : 'work_through' ?>"
       data-resume="<?= $poser->work_through_pose_step == $poser->pose_count()
         ? 'last' : $poser->work_through_pose_step + 1 ?>">
    &raquo;
  </nav>
</div>

<script>
  update_search_link();

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
