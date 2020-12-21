#include "common.h"
#include "glossary.h"
#include "stringview.h"
#include "set.h"
#include "map.h"

struct Food {
    IntSet_t ingredients;
    IntSet_t allergens;
};

int main (int argc, char ** argv) {
    Glossary_t glossary;
    glossary_init(&glossary);

    size_t food_count = 0;
    struct Food food [64];

    IntSet_t ingredients;
    intset_init(&ingredients, 0);

    IntSet_t allergens;
    intset_init(&allergens, 0);

    do {
        char buf [BUFSIZ];
        char * s = fgets(&(buf[0]), sizeof(buf), stdin);
        if (!s) {
            break;
        }

        StringView_t line = sv_view_c_string(&(buf[0]));
        sv_eat_spaces(&line);
        if (sv_is_empty(&line)) {
            continue;
        }

        StringView_t ingredient_list = sv_eat_until(&line, '(');
        ASSERT(line.start[0] == '(');
        sv_eat_char(&line);
        StringView_t allergen_list = sv_eat_until(&line, ')');
        ASSERT(line.start[0] == ')');
        StringView_t contains = sv_eat_until_space(&allergen_list);
        ASSERT(sv_equals(&contains, "contains"));
        sv_eat_spaces(&allergen_list);

        IntSet_t this_ingredients;
        intset_init(&this_ingredients, 0);

        IntSet_t this_allergens;
        intset_init(&this_allergens, 0);

        while (!sv_is_empty(&ingredient_list)) {
            StringView_t ingredient = sv_eat_until_space(&ingredient_list);
            sv_eat_spaces(&ingredient_list);

            intptr_t ingredient_id = glossary_add(&glossary, ingredient);
            intset_add(&ingredients, ingredient_id);
            intset_add(&this_ingredients, ingredient_id);
        }

        while (!sv_is_empty(&allergen_list)) {
            StringView_t allergen = sv_eat_until_punctuation(&allergen_list);
            if (allergen_list.start[0] == ',') {
                sv_eat_char(&allergen_list);
            }
            sv_eat_spaces(&allergen_list);

            intptr_t allergen_id = glossary_add(&glossary, allergen);
            intset_add(&allergens, allergen_id);
            intset_add(&this_allergens, allergen_id);
        }

        ASSERT(food_count < ARRAYCOUNT(food));
        food[food_count].ingredients = this_ingredients;
        food[food_count].allergens = this_allergens;
        food_count += 1;
    } while (1);

/*
    IntSet_t allergen_free_ingredients;
    intset_init(&allergen_free_ingredients, 0);

    for (intptr_t const * it = intset_cbegin(&ingredients); it != intset_cend(&ingredients); ++it) {
        intptr_t ingredient_id = *it;
        IntSet_t possible_allergens;
        intset_copy(&possible_allergens, &allergens);

        for (size_t i = 0; i < food_count; ++i) {
            if (intset_contains(&(food[i].ingredients), ingredient_id)) {
                IntSet_t new_possible_allergens = intset_intersection(&possible_allergens, &(food[i].allergens));
                intset_free(&possible_allergens);
                possible_allergens = new_possible_allergens;

                if (possible_allergens.count == 0) {
                    break;
                }
            }
        }

        if (possible_allergens.count == 0) {
            intset_add(&allergen_free_ingredients, ingredient_id);
        }
        intset_free(&possible_allergens);
    }

    printf("Ingredients that cannot contain allergens:\n");
    intset_print(&allergen_free_ingredients);
*/

    IntMap_t ingredient_to_allergen_map;
    intmap_init(&ingredient_to_allergen_map, allergens.count);
    IntMap_t allergen_to_ingredient_map;
    intmap_init(&allergen_to_ingredient_map, allergens.count);

    int changes;
    do {
        changes = 0;

        for (intptr_t const * it = intset_cbegin(&allergens); it != intset_cend(&allergens); ++it) {
            intptr_t allergen_id = *it;
            if (intmap_contains(&allergen_to_ingredient_map, allergen_id)) {
                continue;
            }

            IntSet_t possible_ingredients;
            intset_copy(&possible_ingredients, &ingredients);

            for (size_t i = 0; i < ingredient_to_allergen_map.count; ++i) {
                intset_remove(&possible_ingredients, ingredient_to_allergen_map.keys[i]);
            }

            for (size_t i = 0; i < food_count; ++i) {
                if (intset_contains(&(food[i].allergens), allergen_id)) {
                    IntSet_t new_possible_ingredients = intset_intersection(&possible_ingredients, &(food[i].ingredients));
                    intset_free(&possible_ingredients);
                    possible_ingredients = new_possible_ingredients;

                    ASSERT(possible_ingredients.count >= 1);
                    if (possible_ingredients.count == 1) {
                        break;
                    }
                }
            }

            if (possible_ingredients.count == 1) {
                intmap_set(&ingredient_to_allergen_map, possible_ingredients.values[0], allergen_id);intmap_set(&allergen_to_ingredient_map, allergen_id, possible_ingredients.values[0]);
                ++changes;
            }
            intset_free(&possible_ingredients);
        }
    } while (changes);

    IntSet_t allergen_free_ingredients;
    intset_copy(&allergen_free_ingredients, &ingredients);
    for (size_t i = 0; i < ingredient_to_allergen_map.count; ++i) {
        intset_remove(&allergen_free_ingredients, ingredient_to_allergen_map.keys[i]);
    }

    size_t answer = 0;
    for (intptr_t const * it = intset_cbegin(&allergen_free_ingredients); it != intset_cend(&allergen_free_ingredients); ++it) {
        for (size_t i = 0; i < food_count; ++i) {
            if (intset_contains(&(food[i].ingredients), *it)) {
                ++answer;
            }
        }
    }
    DISP(answer);
}
