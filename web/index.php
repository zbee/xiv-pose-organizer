<?php
require '../logic/autoload.php';
$poser = new pose_organizer();
?>

<!DOCTYPE html>
<html lang="en">
<head>
  <!--Looking for it-->
  <title>Organize yer Poses</title>
  <link rel="stylesheet" href="assets/style/main.css">
</head>
<body>
<div id="loading">
  <span id="loader">&#9187;</span>
</div>
</body>

<script src="assets/js/jquery.js"></script>
<script src="assets/js/configure.js"></script>
<script src="assets/js/work_through.js"></script>
<script src="assets/js/utilities.js"></script>
<script type="application/javascript">
  function load(page = false, resume = 0) {
    body.find('#loading').show();
    if (page === false) {
      page = '<?=steps[$poser->work_through_step]?>';
    }
    let data = {};
    data[page] = '';

    <?php
      echo "data['resume'] = " . $poser->work_through_pose_step . ';';
    ?>

    if (resume !== -1) {
      data['resume'] = resume;
    }

    console.log('load:' + page);

    $.ajax(
      {
        url: 'middleman.php',
        type: 'GET',
        data: data,
        async: true,
      }
    ).done(
      function (html) {
        $('body').html(html);
        body.find('#loading').hide();
      }
    );
  }

  load();
</script>
</html>
