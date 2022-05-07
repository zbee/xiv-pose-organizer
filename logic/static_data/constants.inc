<?php
/** @noinspection SpellCheckingInspection */
/** @noinspection PhpUnused */

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

const races           = [
  'miqote',
  'highlander',
  'diskwight',
  'roegadyn',
  'plainsfolk',
  'aura',
  'hrothgar',
  'bunny',
];
const race_miqote     = ['miqote', "miqo'te", 'miqo',];
const race_highlander = ['highlander', 'midlander', 'hyur',];
const race_diskwight  = ['diskwight', 'wildwood', 'elezen',];
const race_roegadyn   = ['roegadyn', 'roe',];
const race_plainsfolk = [
  'plainsfolk',
  'dunesfolk',
  'lalafell',
  'potato',
  'lala',
];
const race_aura       = ['aura', 'au ra', 'raen', 'xaela'];
const race_hrothgar   = ['hrothgar', 'hroth',];
const race_bunny      = ['bunny', 'veena', 'viera', 'rava', 'bun',];

const nested_structure = 'folders';
const flat_structure   = 'files';

const this_file  = true;
const other_file = false;

const skip = true;