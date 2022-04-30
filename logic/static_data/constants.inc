<?php

const steps                 = [
  'configure',
  'configure_tags',
  'configure_review',
  'work_through',
  'review',
  'done',
];
const step_configure        = 0;
const step_configure_tags   = 1;
const step_configure_review = 2;
const step_work_through     = 3;
const step_review           = 4;
const step_done             = 5;

const file_format_ana = 'anamnesis';
const file_format_cmt = 'concept_matrix';

const file_extension_ana = 'pose';
const file_extension_cmt = 'cmp';

const genders = [
  'female',
  'male',
];

const gender_male   = true;
const gender_female = false;

const races       = [
  'miqo',
  'hyur',
  'elezen',
  'roe',
  'lala',
  'aura',
  'hroth',
  'bun',
];
const race_miqo   = ['miqo', "miqo'te", 'miqote'];
const race_hyur   = ['hyur', 'midlander', 'highlander'];
const race_elezen = ['elezen', 'wildwood', 'diskwight'];
const race_roe    = ['roe', 'roegadyn'];
const race_lala   = ['lala', 'lalafell', 'potato', 'plainsfolk', 'dunesfolk'];
const race_aura   = ['aura', 'au ra', 'dragon', 'raen', 'xaela'];
const race_hroth  = ['hroth', 'hrothgar', 'lion'];
const race_bun    = ['bun', 'viera', 'bunny', 'rava', 'veena'];

const this_file = true;
const other_file = false;

const skip = true;