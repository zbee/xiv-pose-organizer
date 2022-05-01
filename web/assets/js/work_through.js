// Navigate between poses - previous, next, skip to next
let navigate_attempts = 1;
body.on('click', '#work_through .navigate', function () {
  // Process navigation data
  let data = {
    // Update settings
    _categories: [],
    _tags: [],
    // Update assignments in pose
    categories: [],
    tags: [],
  };
  let navigate_data = $(this).data();
  data[navigate_data.current] = ''
  data[navigate_data.target] = '';
  data['resume'] = navigate_data.resume;
  if (typeof navigate_data.skip != 'undefined')
    data['skip'] = '';
  let request_type = 'POST';
  if (navigate_data.target != 'work_through')
    request_type = 'GET';

  // Don't allow navigation if the fields are empty, with triple click bypass
  if (missing_require_fields() === true && navigate_attempts < 3) {
    navigate_attempts++;
    return;
  } else if (missing_require_fields() === true && navigate_attempts >= 3) {
    navigate_attempts = 1;
    $.ajax(
      {
        url: 'middleman.php',
        type: request_type,
        data: data,
        async: false,
      }
    ).done(
      function (response) {
        load(navigate_data.target, navigate_data.resume);
      }
    );
    return;
  }

  body.find('#loading').show();

  // Add all input fields to the data
  body.find('input[name]').each(function () {
    let name = $(this).attr('name');
    let value = $(this).val();

    if (name == 'pack')
      value = $(this).parent().parent().find('input:checked').val()

    if (name == 'has_preview')
      value = value !== '0';

    if (name != 'file')
      data[name] = value;
  });

  // Add all selected option fields to the data
  body.find('select').each(function () {
    let name = $(this).attr('name');
    let value = $(this).find('option:selected').attr('value');
    if (typeof value === 'undefined')
      value = null;

    // Handle categories and tags separately
    if (name.startsWith('categories') || name.startsWith('tags')) {
      // The pose's tag/category
      data[$(this).attr('class')][name.charAt(name.length - 1)] = value;

      // Updating all of the categories/tags available
      $(this).find('option').each(function () {
        // Establish the array
        if (typeof data['_' + name.slice(0, -1)][name.charAt(name.length - 1)] == 'undefined')
          data['_' + name.slice(0, -1)][name.charAt(name.length - 1)] = [];

        data['_' + name.slice(0, -1)][name.charAt(name.length - 1)].push($(this).val());
      });
    } else if (name.startsWith('verb')) {
      data[name.slice(0, -1)] = value;
      data[name] = [];
      $(this).find('option').each(function () {
        data[name].push($(this).val());
      });
    } else if (name.startsWith('pack_name')) {
      data[name.slice(0, -1)] = value;
      data[name] = {};
      $(this).find('option').each(function () {
        data[name][$(this).val()] = $(this).text();
      });
    } else {
      if (name === 'submission-to-other')
        value = value !== '0';
      data[name] = value;
    }
  });

  // Load the next page
  body.html('<img src="assets/img/loading.gif" alt="Loading...">');
  // Request the next page
  $.ajax(
    {
      url: 'middleman.php',
      type: request_type,
      data: data,
      async: false,
    }
  )
    // Draw the next page
    .done(
      function (response) {
        load(navigate_data.target, navigate_data.resume);
      }
    );
});

// Hide pack-based controls if it's not a pack
body.on('click', 'input[name="pack"]', function () {
  body.find('#data .pack-dependent').toggle($(this).val() == 1);
});

// Add items to <select> lists
body.on('click', '.add_category', function () {
  let select = $(this).parent().parent().find('select');
  let value = $(this).parent().find('input').val();
  let text = value;
  let skip = false;

  // Generate a key for the pack
  if (select.attr('id') === 'pack_names') {
    value = md5(isoTime() + value);
  }

  // Make sure there are not duplicate entries
  select.find('option').each(function () {
    if ($(this).val() === value || $(this).text() === text) {
      alert('Duplicate entry');
      skip = true;
    }
  });

  // Make sure this entry is not empty
  if (value === '')
    skip = true;

  if (!skip)
    select.append(
      '<option value="' + value + '">' + text + '</option>'
    );
});

// Keep others-required and others-posed in sync
body.on('input', '#other_people_required', function () {
  let posed = $(this).parent().find('input[name="other_people_posed"]')
  posed.attr('min', $(this).val());
  if (posed.val() < $(this).val())
    posed.val($(this).val());
})
body.on('input', '#other_people_posed', function () {
  let posed = $(this).parent().find('input[name="other_people_required"]')
  posed.attr('max', $(this).val());
  if (posed.val() > $(this).val())
    posed.val($(this).val());
})

// Enforce category selections when a pose pack is selected
body.on('input', '#pack_names', function () {
  body.find('#loading').show();
  $.ajax(
    {
      url: '/handle/work_through_pack_dependent_data.php',
      type: 'POST',
      data: {'key': $(this).val()[0]},
      async: false,
    }
  ).done(
    function (response) {
      body.find('#loading').hide();
      if (response.length === 0)
        return;
      response = JSON.parse(response);

      body.find('input[name="link"]').val(response.link).prop('disabled', true);
      body.find('input[name="author"]').val(response.author).prop('disabled', true);
      body.find('input[name="other_people_posed"]').val(response.others_posed).prop('disabled', true);

      $.each(response.categories, function (key, value) {
        body.find('select[name="categories' + key + '"]')
          .find('option').each(function () {
          if ($(this).val() === value) {
            $(this).prop('selected', true);
            $(this).parent().prop('disabled', true);
          } else
            $(this).prop('selected', false);
        });
      });
    })
});

// Add additional categories or tags column
body.on('click', '.add_column', function () {
  let column_type = $(this).data('type');
  let index = $(this).data('index');
  $(this).parent().html(
    '<label for="' + column_type + index + '" style="display:none">' +
    'Choose ' + column_type + ' ' + index +
    '</label><select name="' + column_type + index + '" id="' + column_type + index +
    '" data-column="' + index + '" class="' + column_type + '" multiple></select>' +
    '<label><input type="text"/><a class="add_category">Add </a></label>'
  );
});

// Require fields
function missing_require_fields() {
  let required = [
    body.find('input[name="name"]'),
    body.find('input[name="author"]'),
    body.find('input[name="link"]'),
    body.find('select[name="gender"]'),
    body.find('select[name="race"]'),
    body.find('select[name="categories0"]'),
    body.find('select[name="categories1"]'),
    body.find('select[name="categories2"]'),
    body.find('select[name="tags0"]'),
    body.find('select[name="tags1"]'),
    body.find('select[name="tags2"]'),
    body.find('select[name="verbs"]'),
  ];
  let required_pack = [
    body.find('input[name="other_people_required"]'),
    body.find('input[name="other_people_posed"]'),
    body.find('select[name="submission_to_other"]'),
    body.find('select[name="pack_names"]'),
  ];
  let fail = true;
  let return_value = false;

  let pack = body.find('input[id="pack_yes"]').prop('checked');

  $.each(required, function (trash, selector) {
    if (selector.length === 0)
      return;
    if (selector.val().length === 0) {
      selector.addClass('error');
      return_value = fail;
    } else
      selector.removeClass('error');
  });

  if (pack) {
    $.each(required_pack, function (trash, selector) {
      if (selector.val().length === 0) {
        selector.addClass('error');
        return_value = fail;
      } else
        selector.removeClass('error');
    });
  }

  return return_value;
}

// Remove error class
body.on('input', 'input, select', function () {
  navigate_attempts = 1;
  $(this).removeClass('error');
});

// Update XIVMA search link
body.on('input', 'input[name="name"]', function () {
  update_search_link('xivma_search', 'input[name="name"]');
});
body.on('input', 'select[name="pack_names"]', function () {
  update_search_link('xivma_pack_search', 'select[name="pack_names"]');
});

function update_search_link(link, input) {
  let value = body.find(input).val();
  if (input.startsWith('select'))
    value = body.find(input).find('option:selected').text();

  body.find('a[id="' + link + '"]').attr(
    'href',
    body.find('a[id="' + link + '"]').data('link') + value
  );
}