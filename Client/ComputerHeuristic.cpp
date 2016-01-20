#include <stdio.h>

#define POTENTIAL 1
#define LINE 2

#define WIDTH 19
#define POWER 5

void deconstruct(unsigned char *map, unsigned char *weights_map, int index_origin, int index_tile, int dirx, int diry) {
  int i = index_origin + (POWER - 1) * -dirx + (POWER - 1) * -diry * WIDTH;
  int last = index_tile + POWER * dirx + POWER * diry * WIDTH;

  for (i; i < last; i += dirx + diry * WIDTH) {
    if (i >= 0 && i < 49 && map[i] != map[index_origin]) {
      weights_map[i] -= LINE;
    }
  }
}

void update_tile(unsigned char *map, unsigned char *weights_map, int n) {
  int i, j, k, ok, destroyed_neg, destroyed_pos;

  static int dirx[4] = {1, 0, 1, 1};
  static int diry[4] = {0, 1, 1, -1};
  // Rows
  int destroyed[40] = {0};
  for (k = 0; k < 4; k++) {
    destroyed_neg = -POWER - 1;
    destroyed_pos = POWER + 1;
    for (i = -POWER + 1; i < 1; i++) {
      ok = 0;
      for (j = 0; j < POWER; j++) {
        int index = n + (i + j) * dirx[k] + ((i + j) * WIDTH) * diry[k];
        if (index >= 0 && index < 49) {
          int tile = map[index];
          if (tile == 255)
            ++ok;
          else if (tile == map[n]) {
            ++ok;
            if (i + j > 0 && destroyed_pos > i + j)
              destroyed_pos = i + j;
            else if (i + j < 0 && destroyed_neg < i + j)
              destroyed_neg = i + j;
          }
          else if (((destroyed_pos > i + j && i + j > 0) || (destroyed_neg < i + j && i + j < 0)) && destroyed[i + j + 10] == 0) {
            destroyed[i + j + 10] = 1;
            deconstruct(map, weights_map, n, index, i < 0 ? dirx[k] : -dirx[k], i < 0 ? diry[k] : -diry[k]);
          }
        }
      }
      if (ok >= POWER && map[n] != 255) {
        for (j = 0; j < POWER; j++)
          weights_map[n + (i + j) * dirx[k] + ((i + j) * WIDTH) * diry[k]] += LINE;
      }
    }
  }
}

// Updates all the map
void calc_initial_weight(unsigned char *map, unsigned char *weights_map) {
  int i;
  for (i = 0; i < WIDTH * WIDTH; i++) {
    int j, ok;
    // Rows
    ok = 0;
    for (j = 1; j < POWER; j++) {
      if (i / WIDTH == (i + j) / WIDTH) { // i.e same row
        weights_map[i] += POTENTIAL;
        weights_map[i + j] += POTENTIAL;
        if (map[i + j] == 255 || map[i + j] == map[i]) // i.e same color OR blank
          ++ok;
      }
    }
    if (ok == POWER - 1 && map[i] != 255) {
      for (j = 1; j < POWER; j++)
        weights_map[j + i] += LINE;
    }
    // Columns
    ok = 0;
    for (j = 1; j < POWER; j++) {
      if ((i + j * WIDTH) < 49) {
        weights_map[i] += POTENTIAL;
        weights_map[i + j * WIDTH] += POTENTIAL;
        if (map[i + j * WIDTH] == 255 || map[i + j * WIDTH] == map[i])
          ++ok;
      }
    }
    if (ok == POWER - 1 && map[i] != 255) {
      for (j = 1; j < POWER; j++)
        weights_map[i + j * WIDTH] += LINE;
    }
    // Down + Right Diagonal
    ok = 0;
    for (j = 1; j < POWER; j++) {
      if ((i + j + j * WIDTH) < 49 && (i + j + j * WIDTH) / WIDTH == i / WIDTH + j) { // Tile = i + j + j * WIDTH
        weights_map[i] += POTENTIAL;
        weights_map[i + j + j * WIDTH] += POTENTIAL;
        if (weights_map[i + j + j * WIDTH] == 255 || weights_map[i + j + j * WIDTH] == weights_map[i])
          ++ok;
      }
    }
    if (ok == POWER - 1 && map[i] != 255) {
      for (j = 1; j < POWER; j++)
        weights_map[i + j + j * WIDTH] += LINE;
    }
    // Up + Right Diagonal
    ok = 0;
    for (j = 1; j < POWER; j++) {
      if ((i + j - j * WIDTH) >= 0 && (i + j - j * WIDTH) / WIDTH == i / WIDTH - j) {
        weights_map[i] += POTENTIAL;
        weights_map[i + j - j * WIDTH] += POTENTIAL;
        if (weights_map[i + j - j * WIDTH] == 255 || weights_map[i + j - j * WIDTH] == weights_map[i])
          ++ok;
      }
    }
    if (ok == POWER - 1 && map[i] != 255) {
      for (j = 1; j < POWER; j++)
        weights_map[i + j - j * WIDTH] += LINE;
    }
  }
}

void display_weights_map(unsigned char *weights_map, unsigned char *map) {
  int i, j = 0;
  for (i = 0; i < WIDTH * WIDTH; i++) {
    printf("%2d ", weights_map[i]);
    if (i % 7 == 6) {
      printf("     %3d ", map[j]);
      j++;
      for (j; j % 7 != 0; j++)
        printf("%3d ", map[j]);
      printf("\n");
    }
  }
}
