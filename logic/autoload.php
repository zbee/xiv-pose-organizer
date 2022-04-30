<?php

// Load utilities
require 'utilities/utilities.inc';

// Load configuration and setup
require 'static_data/config.inc';
require 'static_data/constants.inc';

require 'structures/pose.inc';
require 'structures/settings.inc';

// Load core included code
require 'core/folder_parser.inc';
require 'core/maintain_tagging.inc';
require 'core/navigator.inc';
require 'core/save_pose.inc';
require 'core/core.inc';

// Load including code
require 'checks/pack.inc';
require 'checks/file_structure.inc';
require 'checks/tags.inc';

require 'review/configuration.inc';
require 'review/race.inc';
require 'review/file_structure.inc';