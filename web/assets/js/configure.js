let body = $('body');

function clear_select () {
  $('#configure select option').each(function () {
    $(this).prop('selected', false);
  });
}

function load_settings(settings) {
  console.log('load:settings');
  $.each(settings, function (setting, value) {
    switch (setting) {
      case 'poses_folder':
        $('#config-pose_folder input').val(value);
        break;
      case 'format_for_pose_normalization':
        clear_select();
        $('#configure select option[value="' + value + '"]')
          .prop('selected', true);
        break;
      case 'categories':
      case 'tags':
        //server side
        break;
      default:
        $('#configure')
          .find('input[data-key="' + setting + '"]')
          .prop('checked', value === 'true');
    }
  });
}

//region First page
jQuery.fn.just_text = function () {
  return $(this).clone()
    .children()
    .remove()
    .end()
    .text();
};

body.on('click', '#configure a.load_lazy', function () {
  $('#configure input[type="checkbox"]').each(function () {
    $(this).prop('checked', false);
  })
});
body.on('click', '#configure a.load_tryhard', function () {
  $('#configure input[type="checkbox"]').each(function () {
    $(this).prop('checked', true);
  });
  $('#configure input[data-key="always_attribute_to_author"]')
    .prop('checked', false);
  clear_select();
  $('#configure select option[value="ana"]').prop('selected', true);
});

function save(target, data, page) {
  console.log('save:' + target);
  data[target] = ''

  $.ajax(
    {
      url: 'middleman.php',
      type: 'POST',
      data: data,
      async: true,
    }
  ).done(
    function (response) {
      load(page);
    }
  );
}

body.on('click', '#configure .navigate', function () {
  if (
    typeof $(this).attr('data-current') !== 'undefined'
    && $(this).attr('data-current') !== false
  ) {
    let data = {}
    // Save the basic config page data
    if ($(this).parent().find('#body label').length > 0) {
      $(this).parent().find('#body label').each(
        function () {
          if ($(this).find('select').length > 0)
            data['format_for_pose_normalization'] =
              $(this).find('select').val();
          if ($(this).find('input[type="text"]').length > 0)
            data['poses_folder'] = $(this).find('input[type="text"]').val();

          if ($(this).find('input[type="checkbox"]').length > 0)
            data[$(this).find('input[type="checkbox"]').data('key')] =
              $(this).find('input[type="checkbox"]').is(":checked");
        }
      );
    }
    // Save the tags page data
    else if ($(this).parent().find('#body #categories div').length > 0) {
      data = {categories: [], tags: []}
      $(this).parent().find('#body #categories div').each(
        function (index) {
          $(this).find('.tag').each(
            function (trash) {
              if (typeof (data['categories'][index]) === 'undefined')
                data['categories'][index] = []
              data['categories'][index].push($(this).just_text().trim());
            }
          );
        }
      );
      $(this).parent().find('#body #tags div').each(
        function (index) {
          $(this).find('.tag').each(
            function (trash) {
              if (typeof (data['tags'][index]) === 'undefined')
                data['tags'][index] = []
              data['tags'][index].push($(this).just_text().trim());
            }
          );
        }
      );
    }
    save($(this).data('current'), data, $(this).data('target'))
  } else {
    load($(this).data('target'));
  }
});
//endregion First page

//region Second page
function add_tag() {
  return '<a class="add_tag"><span style="font-weight:900">&plus;</span> Add</a>';
}

function remove_button() {
  return '<a style="font-weight:900;color:#E53935" ' +
    'onclick="$(this).parent().parent().remove()">&#10005;</a>';
}

function tag(name) {
  return '<span style="display:block" class="tag">' + name + ' <span>'
    + remove_button() + '</span></span>';
}

function add_category(element, tags = false) {
  let column_holder = element.parent().parent().children('.column_holder');
  let columns = column_holder.children().length;
  if (columns >= 3)
    element.parent().remove();

  let title;
  if (column_holder.attr('id') === 'tags')
    title = 'Tag ' + (columns + 1).toString()
  else
    title = 'Category ' + (columns + 1).toString()

  if (tags !== false) {
    let tag_text = '';
    $.each(tags, function(trash, tag_name) {
      tag_text = tag_text + tag(tag_name)
    });
    tags = tag_text;
  } else {
    tags = '';
  }

  column_holder.append(
    '<div style="display:inline-block;grid-row:1"><h4>' + title + ' ' +
    remove_button() + '</h4>' +
    tags +
    add_tag() +
    '</div>'
  );
}

body.on('click', '#configure a.populate_defaults', function () {
  //region Category population
  let categories = $('#categories');
  let number_of_categories = categories.children().length;
  categories.append(
    '<div style="display:inline-block;grid-row:1"><h4>Category ' +
    (number_of_categories + 1).toString() + ' ' + remove_button() + '</h4>' +
    tag('nsfw') +
    tag('lewd') +
    tag('sfw') +
    add_tag() +
    '</div>' +
    '<div style="display:inline-block;grid-row:1"><h4>Category ' +
    (number_of_categories + 2).toString() + ' ' + remove_button() + '</h4>' +
    tag('erotic') +
    tag('cute') +
    tag('adventure') +
    tag('dance') +
    tag('romantic') +
    tag('sad') +
    tag('idle') +
    add_tag() +
    '</div>'
  );
  //endregion Category population

  //region Tag population
  let tags = $('#tags');
  let number_of_tags = tags.children().length;
  tags.append(
    '<div style="display:inline-block;grid-row:1"><h4>Tag ' +
    (number_of_tags + 1).toString() + ' ' + remove_button() + '</h4>' +
    tag('clothed') +
    tag('smalls') +
    tag('naked') +
    add_tag() +
    '</div>' +
    '<div style="display:inline-block;grid-row:1"><h4>Tag ' +
    (number_of_tags + 2).toString() + ' ' + remove_button() + '</h4>' +
    tag('sit') +
    tag('stand') +
    tag('kneel') +
    tag('lay') +
    add_tag() +
    '</div>'
  );
  //endregion Tag population

  $(this).remove();
});

body.on('click', '#configure .add_category', function () {
  add_category($(this));
});

body.on('click', '#configure a.add_tag', function () {
  $(this).parent().append(
    '<input type="text" placeholder="press enter to save"' +
    ' class="add_tag_typed" style="width:110px">'
  );
  $(this).remove();
});

body.on('keyup', '#configure input.add_tag_typed', function (e) {
  if (e.keyCode === 13) {
    let tag_name = $(this).val();
    $(this).parent().append(tag(tag_name) + add_tag());
    $(this).remove();
  }
});
//endregion Second page