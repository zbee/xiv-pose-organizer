<?php

class structure_swap extends save_pose {

  /**
   * @var string path to working directory
   */
  private string $path;

  /**
   * @var bool new_structure to switch to
   */
  private bool $new_structure;

  /**
   * @var DirectoryIterator[] list of folders to be swapped
   */
  private array $folders = [];

  /**
   * @var string[][] list of files to swap for a single pose
   */
  private array $files = [];

  /**
   * @var int ID of the current VarVar to swap
   */
  private int $current = 0;

  /**
   * @var string VarVar of 'files' or 'folders'
   */
  private string $swapping_from = '';

  /**
   * @var string VarVar of 'files' or 'folders'
   */
  private string $swapping_to = '';

  public function swap($new_structure): void {
  }

  private function swap_name(): string|array {
    return 'Unimplemented';
  }

  private function move_to_folder(): void {
  }

  private function move_to_loose(): void {
  }

  private function clean_up(): void {
  }

}