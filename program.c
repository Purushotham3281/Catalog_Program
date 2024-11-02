#include <stdio.h>
#include <stdlib.h>
#include <json-c/json.h>
#include <gmp.h>

void decode_value(mpz_t result, const char *value, int base) {
    mpz_set_str(result, value, base);
}

void lagrange_interpolation(mpz_t constant_term, int *x, mpz_t *y, int k) {
    mpz_set_ui(constant_term, 0);

    for (int i = 0; i < k; i++) {
        mpz_t term, temp;
        mpz_init_set(term, y[i]);
        mpz_init(temp);

        for (int j = 0; j < k; j++) {
            if (i != j) {
                mpz_set_si(temp, -x[j]);
                mpz_mul(term, term, temp);
                
                mpz_set_si(temp, x[i] - x[j]);
                mpz_divexact(term, term, temp);
            }
        }
        mpz_add(constant_term, constant_term, term);
        mpz_clear(term);
        mpz_clear(temp);
    }
}

int main() {
    FILE *file = fopen("input.json", "r");
    if (!file) {
        printf("Error: Could not open JSON file\n");
        return 1;
    }

    struct json_object *parsed_json;
    parsed_json = json_object_from_file("input.json");

    struct json_object *keys;
    struct json_object_object_get_ex(parsed_json, "keys", &keys);
    int n = json_object_get_int(json_object_object_get(keys, "n"));
    int k = json_object_get_int(json_object_object_get(keys, "k"));

    int x[k];
    mpz_t y[k];
    for (int i = 0; i < k; i++) {
        mpz_init(y[i]);
    }

    int count = 0;
    json_object_object_foreach(parsed_json, key, val) {
        if (strcmp(key, "keys") != 0 && count < k) {
            int x_value = atoi(key);
            struct json_object *base_obj = json_object_object_get(val, "base");
            struct json_object *value_obj = json_object_object_get(val, "value");

            int base = json_object_get_int(base_obj);
            const char *value = json_object_get_string(value_obj);

            x[count] = x_value;
            decode_value(y[count], value, base);
            count++;
        }
    }

    mpz_t constant_term;
    mpz_init(constant_term);

    lagrange_interpolation(constant_term, x, y, k);

    gmp_printf("Constant term (secret c): %Zd\n", constant_term);

    mpz_clear(constant_term);
    for (int i = 0; i < k; i++) {
        mpz_clear(y[i]);
    }
    json_object_put(parsed_json);
    fclose(file);

    return 0;
}
