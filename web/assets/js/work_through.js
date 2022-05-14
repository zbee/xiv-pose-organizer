// Navigate between poses - previous, next, skip to next
let navigate_attempts = 1;

function navigate(nav) {  // Process navigation data
  let data = {
    // Update settings
    _categories: [],
    _tags: [],
    // Update assignments in pose
    categories: [],
    tags: [],
  };
  let navigate_data = nav.data();

  if (typeof navigate_data.find_incomplete !== 'undefined') {
    data['find_incomplete'] = '';
    $.ajax(
      {
        url: 'middleman.php',
        type: 'GET',
        data: data,
        async: false,
      }
    ).done(
      function (response) {
        load(navigate_data.target, response);
      }
    );
    return;
  }

  data[navigate_data.current] = ''
  data[navigate_data.target] = '';
  data['resume'] = navigate_data.resume;
  if (typeof navigate_data.skip != 'undefined') {
    data['skip'] = '';
    data['key'] = navigate_data.key;
  }
  let request_type = 'POST';
  if (navigate_data.target != 'work_through')
    request_type = 'GET';

  // Don't allow navigation if the fields are empty, with triple click bypass
  if (missing_required_fields() === true && navigate_attempts < 3 && typeof navigate_data.reset === 'undefined' && typeof navigate_data.skip === 'undefined') {
    navigate_attempts++;
    return;
  } else if (typeof navigate_data.reset !== 'undefined' || typeof navigate_data.skip !== 'undefined' || (missing_required_fields() === true && navigate_attempts >= 3)) {
    navigate_attempts = 1;
    body.find('#loading').show();
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
    var name = $(this).attr('name');
    var value = $(this).val();

    if (name == 'pack')
      value = $(this).parent().parent().find('input:checked').val()

    if (name == 'has_preview')
      value = value !== '0';

    if (name != 'file')
      data[name] = value;
  });

  // Add all selected option fields to the data
  body.find('select').each(function () {
    name = $(this).attr('name');
    value = $(this).find('option:selected').attr('value');
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
        value = value === '' ? null : value !== '0';
      data[name] = value;
    }
  });

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
        body.find('#loading').hide();
        load(navigate_data.target, navigate_data.resume);
      }
    );
}

body.on('keydown', '.navigate:focus', function (e) {
  if (e.keyCode == 13 || e.keyCode == 32) {
    if (typeof $(this).attr('data-copy') !== 'undefined') {
      copy_pose($(this).attr('data-copy'));
      return;
    }

    navigate($(this));
  }
});

// Hide pack-based controls if it's not a pack
body.on('click', 'input[name="pack"]', function () {
  body.find('.pack-dependent').toggle($(this).val() == 1);
});

// Add items to <select> lists
function add_category(cat) {
  let select = cat.parent().parent().find('select');
  let value = cat.parent().find('input').val();
  let text = value;
  let skip = false;

  // Generate a key for the pack
  if (select.attr('id') === 'pack_names') {
    value = md5(isoTime() + value);
  }

  // Make sure there are not duplicate entries
  select.find('option').each(function () {
    if (cat.val() === value || cat.text() === text) {
      alert('Duplicate entry');
      skip = true;
    }
  });

  // Make sure this entry is not empty
  if (value === '')
    skip = true;

  if (!skip) {
    cat.parent().find('input').val('')
    if (select.attr('id') === 'pack_names') {
      select.prepend(
        '<option value="' + value + '">' + text + '</option>'
      );
    } else {
      select.append(
        '<option value="' + value + '">' + text + '</option>'
      );
    }
  }
}

body.on('keydown', '.add_category:focus', function (e) {
  if (e.keyCode == 13 || e.keyCode == 32) {
    add_category($(this));
  }
});


// Keep others-required and others-posed in sync
body.on('input', '#other_people_required', function () {
  let posed = $(this).parent().parent().find('input[name="other_people_posed"]')
  posed.attr('min', $(this).val());
  if (posed.val() < $(this).val())
    posed.val($(this).val());
})
body.on('input', '#other_people_posed', function () {
  let posed = $(this).parent().parent().find('input[name="other_people_required"]')
  posed.attr('max', $(this).val());
  if (posed.val() > $(this).val())
    posed.val($(this).val());
})

// Enforce category selections when a pose pack is selected
function pack_name_extra_load(element, keycode, image = false) {
  body.find('#loading').show();
  $.ajax(
    {
      url: '/handle/work_through_pack_dependent_data.php',
      type: 'POST',
      data: {'key': element.val()[0]},
      async: true,
    }
  ).done(
    function (response) {
      body.find('#loading').hide();

      // Clear disables when nothing is returned
      if (response.length === 0) {
        // if there were disabled fields, clear them
        body.find('*[disabled]').each(function () {
          $(this).val('');
          $(this).find('option').each(function () {
            $(this).removeAttr('selected');
          });
        });

        body.find('*[disabled]').removeAttr('disabled');
        return;
      }
      response = JSON.parse(response);

      body.find('div[id="image"]').html(response.image);

      body.find('input[name="link"]').val(response.link)
        .prop('disabled', true).removeClass('error');
      body.find('input[name="author"]').val(response.author)
        .prop('disabled', true).removeClass('error');
      body.find('input[name="other_people_posed"]').val(response.others_posed)
        .prop('disabled', true).removeClass('error');

      $.each(response.categories, function (key, value) {
        body.find('select[name="categories' + key + '"]')
          .find('option').each(function () {
          if ($(this).val() === value) {
            $(this).prop('selected', true);
            $(this).parent().prop('disabled', true).removeClass('error');
          } else
            $(this).prop('selected', false);
        });
      });

      if (keycode == 9) {
        element.parent().find('input').focus();
      } else {
        element.parent().parent().find('input[name="pack"]').focus();
      }
    })
}

body.on('click', '#pack_names', function () {
  pack_name_extra_load($(this));
});
body.on('keydown', '#pack_names', function (e) {
  if (
    (e.keyCode == 9 || e.keyCode == 16) &&
    $(this).find('option:selected').length > 0
  ) {
    pack_name_extra_load($(this), e.keyCode);
  }
});

// Add additional categories or tags column
function add_column(col) {
  let column_type = col.data('type');
  let index = col.data('index');
  let tab = parseInt(col.data('tab')) + parseInt(index);
  let tab_long = 30 + parseInt(col.data('tab')) + parseInt(index) * 10;
  col.parent().html(
    '<label for="' + column_type + index + '" style="display:none">' +
    'Choose ' + column_type + ' ' + index +
    '</label><select name="' + column_type + index + '" id="' + column_type + index +
    '" data-column="' + index + '" class="' + column_type + '" tabindex="' + tab + '" multiple></select>' +
    '<label><input type="text" tabindex="' + tab_long + '"/><a class="add_category" tabindex="' + (tab_long + 1) + '">Add </a></label>'
  );
}

body.on('keydown', '.add_column:focus', function (e) {
  if (e.keyCode == 13 || e.keyCode == 32) {
    add_column($(this));
  }
});

// Require fields
function missing_required_fields() {
  let required = [
    body.find('input[name="name"]'),
    body.find('input[name="author"]'),
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
  if (required_data.link) {
    required.push(body.find('input[name="link"]'));
  }
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

  if (required_data.preview && body.find('input[name="has_preview"]').val() === '0') {
    body.find('div[id="image"] > div').addClass('error');
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
body.on('input', 'input[name="author"]', function () {
  update_search_link('xivma_author_search', 'input[name="author"]');
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

// Keybinding the path span copy to clipboard
body.on('keydown', 'span:focus', function (e) {
  if (e.keyCode == 13 || e.keyCode == 32) {
    copy_to_clipboard($(this));
  }
});

// Offer author auto-complete
body.on('keydown', '#author', function (e) {
  var author_fill = body.find('#author_fill');
  if (e.keyCode == 9 || e.keyCode == 16) {
    if ($(this).val().length < author_fill.text().length) {
      $(this).val(author_fill.text().slice(0, -1));
      author_fill.html('&nbsp;');
      update_search_link('xivma_author_search', 'input[name="author"]');
    }
  } else {
    $.ajax(
      {
        url: '/handle/work_through_authors.php',
        type: 'POST',
        data: {'author': $(this).val()},
        async: true,
      }
    ).done(
      function (response) {
        author_fill.html(response + '?');
      }
    );
  }
});

// Offer previous-poses field copying
function copy_pose(copy_type) {
  body.find('#loading').show();
  $.ajax(
    {
      url: '/handle/work_through_copy.php',
      type: 'POST',
      data: {'copy_type': copy_type},
      async: true,
    }
  ).done(
    function (response) {
      if (response.length === 0) {
        alert('No similar pose found');
        body.find('#loading').hide();
        return;
      }
      response = JSON.parse(response);

      // Ensure pack is selected
      if (response.pack) {
        body.find('#pack_yes').prop('checked', true);
        body.find('#pack_no').prop('checked', false);
      } else {
        body.find('#pack_yes').prop('checked', false);
        body.find('#pack_no').prop('checked', true);
      }

      // Unselect any selections
      body.find('select').each(function () {
        if ($(this).attr('id') !== 'gender' && $(this).attr('id') !== 'race') {
          $(this).find('option').each(function () {
            $(this).prop('selected', false);
          });
        }
      });

      // Handle author and link
      body.find('input[name="author"]').val(response.author);
      body.find('input[name="link"]').val(response.link);

      // Handle image
      if (response.has_preview) {
        body.find('div[id="image"]').html(
          "<img src='" + response.image_name + "?" + Math.random().toString() + "' alt='Pose preview image' />" +
          "<input name='" + response.has_preview + "' value='1' type='hidden'>" +
          "<input name='image_name' value='" + response.image_name + "' type='hidden'>"
        );
      }

      // Handle pack selection
      var pack_names = body.find('select[name="pack_names"]');
      pack_names.find('option[value="' + response.pack_name + '"]')
        .prop('selected', true).parent().removeClass('error');

      // Fill in the rest of the fields
      body.find('input[name="name"]')
        .val(response.name).removeClass('error');

      switch (response.submission_to_other) {
        case null:
          response.submission_to_other = '';
          break;
        case true:
          response.submission_to_other = '1';
          break;
        case false:
          response.submission_to_other = '0';
          break;
      }
      body.find('select[name="submission_to_other"]')
        .find('option[value="' + response.submission_to_other + '"]')
        .prop('selected', true).parent().removeClass('error');

      body.find('input[name="other_people_posed"]')
        .val(response.other_people_posed).removeClass('error');
      body.find('input[name="other_people_required"]')
        .val(response.other_people_required).removeClass('error');

      if (typeof response.categories[0] !== 'undefined') {
        body.find('select[name="categories0"]')
          .find('option[value="' + response.categories[0] + '"]')
          .prop('selected', true).parent().removeClass('error');
      }

      if (typeof response.categories[1] !== 'undefined') {
        body.find('select[name="categories1"]')
          .find('option[value="' + response.categories[1] + '"]')
          .prop('selected', true).parent().removeClass('error');
      }

      if (typeof response.categories[2] !== 'undefined') {
        body.find('select[name="categories2"]')
          .find('option[value="' + response.categories[2] + '"]')
          .prop('selected', true).parent().removeClass('error');
      }

      if (typeof response.tags[0] !== 'undefined') {
        body.find('select[name="tags0"]')
          .find('option[value="' + response.tags[0] + '"]')
          .prop('selected', true).parent().removeClass('error');
      }

      if (typeof response.tags[1] !== 'undefined') {
        body.find('select[name="tags1"]')
          .find('option[value="' + response.tags[1] + '"]')
          .prop('selected', true).parent().removeClass('error');
      }

      if (typeof response.tags[2] !== 'undefined') {
        body.find('select[name="tags2"]')
          .find('option[value="' + response.tags[2] + '"]')
          .prop('selected', true).parent().removeClass('error');
      }

      body.find('select[name="verbs"]')
        .find('option[value="' + response.verb + '"]')
        .prop('selected', true).parent().removeClass('error');

      body.find('select[name="gender"]').focus();
      body.find('#loading').hide();
    }
  );
}