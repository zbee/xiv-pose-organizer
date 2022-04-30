<?php

$format_for_pose_normalization_options = [
  'anamnesis',
  'concept_matrix',
];

$static_data = [
  'categories' => [
    [ // Category of the pose
      'nsfw',
      'lewd',
      'sfw',
    ],
    [ // Mood of the pose
      'erotic',
      'cute',
      'adventurous',
      'dance',
      'idle',
      'romantic',
      'emo',
      'stretch',
    ]
    // Categories will always have a population tag, with 'alone' or 'group(<x>)'
  ],
  'tags' => [
    [ // Clothing you should be wearing for the pose
      'clothed',
      'smalls',
      'naked',
    ],
    [ // General character positioning
      'sit',
      'stand',
      'kneel',
      'lay',
    ],
    [
      'sub',
      'dom',
      'alone'
    ],
    // Tags always have a wants-other (similar to population from categories, above) tag with
    //   quantity of people /required/ for the pose and whether they assume a sub or dom type
    //   of position
    // Tags always have support for race and gender for poses
    // Author and pose name are handled separately from tags and categories
  ]
];