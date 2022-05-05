<?php

class structure_swap extends save_pose {

  /**
   * @var string path to working directory
   */
  private string $path;

  /**
   * @var string new_structure to switch to, to be used as a VarVar
   */
  private string $new_structure;

  /**
   * @var string structure being switched from, to be used as a VarVar
   */
  private string $old_structure;

  /**
   * @var DirectoryIterator[] list of folders to be swapped
   */
  private array $folders = [];

  /**
   * @var string[][] list of files to swap for a single pose
   */
  private array $files = [];

  public function swap(): void {
    // Determine the new structure
    if ($this->use_nested_structure_for_poses === 'true') {
      $this->new_structure = flat_structure;
    }
    else {
      $this->new_structure = nested_structure;
    }
    $this->old_structure =
      $this->new_structure == flat_structure ? nested_structure
        : flat_structure;

    // Assign working directory
    $this->path = $this->poses_folder . '\\..working';

    // Read working directory
    foreach (new DirectoryIterator($this->path) as $file_folder) {
      // Read folders if going to a flat structure
      if (
        $file_folder->isDir() and
        !$file_folder->isDot() and
        $this->old_structure == nested_structure
      ) {
        $this->folders[] = new DirectoryIterator(
          $file_folder->getPath() . '\\' . $file_folder->getFilename()
        );
      }

      // Read files if going to a nested structure
      if ($file_folder->isFile() && $this->old_structure == flat_structure) {
        // Get file name
        $file_name = $file_folder->getFilename();
        $file_name = explode('.', $file_name);
        unset($file_name[count($file_name) - 1]);
        $file_name = implode('.', $file_name);

        // Set up file holder
        if (!isset($this->files[$file_name])) {
          $this->files[$file_name] = [];
        }

        $this->files[$file_name][] =
          $file_folder->getPath() . '\\' . $file_folder;
      }
    }

    // Generate new names to use
    $this->swap_names();

    // Begin moving files
    foreach ($this->{$this->old_structure} as $index => $files) {
      $this->save_progress($index);
      $this->{'move_to_' . $this->new_structure}();
    }

    // Delete old structure
    $this->clean_up();
  }

  public function save_progress(int $index): void {
    file_put_contents(
      '..\\data\\swap.json',
      json_encode(
        [
          $index,
          $this->new_structure == flat_structure
            ? count($this->files)
            : count(
            $this->folders
          ),
        ]
      )
    );
  }

  private function swap_names() {
    // Determine new file and folder names

    // Sort out new name sets and destination files
    foreach ($this->{$this->old_structure} as $file_folder) {
      if (gettype($file_folder) == 'object') {
        foreach ($file_folder as $file) {
          //$this->{$this->new_structure}[$index] = $file_folder->getPath() . '\\' . $file->getFilename();
        }
      }
      else {

      }
    }
  }

  /** @noinspection PhpUnusedPrivateMethodInspection */
  private function move_to_folders(): void {
  }

  /** @noinspection PhpUnusedPrivateMethodInspection */
  private function move_to_files(): void {
  }

  private function clean_up(): void {
  }

}