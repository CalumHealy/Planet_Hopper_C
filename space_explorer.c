#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "space_explorer.h"

#define DIST_THRESHOLD  1

#ifndef srand48
#define srand48(X) (srand(X))
#endif
#ifndef drand48
#define drand48() (rand()*((double)1/RAND_MAX))
#endif

/**
 * From https://preshing.com/20121224/how-to-generate-a-sequence-of-unique-random-integers/
 */
unsigned int permuteQPR(unsigned int x)
{
    static const unsigned int prime = 4294967291u; // This is the largest prime number that can be represented using an unsigned 32-bit integer
    if (x >= prime) // Any inputted number bigger than prime will not be changed in this function
        return x;  // The 5 integers out of range are mapped to themselves.
    unsigned int residue = ((unsigned long long) x * x) % prime; // Applies the modulo thing to (x squared), an unsigned long long is required because an unsigned int might result in overflow
    return (x <= prime / 2) ? residue : prime - residue; // If (x <= prime / 2) {return residue;}, else {return prime - residue};
}

unsigned int seedPRNG(unsigned int seed){
    return permuteQPR(permuteQPR(seed) + 0x46790905); // Applies the permuteQPR() randomizer, adds the random-ish number 0x46790905, then applies permuteQPR(), ensuring good shuffling and pseudo-randomness
}

unsigned int gen_planet_id(unsigned int id, unsigned int intOffset){
    unsigned int planet_id = RAND_PLANET; // Initialize planet_id as RAND_PLANET defined in space_explorer.h
    while (planet_id == RAND_PLANET){ // While planet_id is still the initial value
        planet_id = permuteQPR((permuteQPR(id) + intOffset) ^ 0x5bf03635); // Generates new planet_id by applying permuteQPR(), adding intOffset, does bitwise XOR operation with number, them applies permuteQPR() again
    }
    return planet_id;
}

double get_distance(Point a, Point b){
    int val = (b.x-a.x)*(b.x-a.x)+(b.y-a.y)*(b.y-a.y); // Pythagora's Theorem to find distance (but val is still the square, a**2)
    return sqrt(val); // Gets the square root and return it
}

/**
 * Generates the planets based on the points
 */
Planet *points_to_planets(Point *points, int num_points){ // Takes in the list of points and the number of points
    int i,j;
    Planet *planets = malloc(num_points*sizeof(Planet)); // Creates a list of planets, length of list is num_points as each point will become a planet
    Planet *connections[num_points]; // Array of pointers to planet objects
    int prng_seed = seedPRNG(1); // Initializes prng_seed using seed randomizer function

    // Create the planets
    for (i=0;i<num_points-1;i++){ // Generating values for each planet object, each of which has ID, point, num_connections, and has_mixer
        planets[i].planet_id = gen_planet_id(i, prng_seed); // Random seed based on position of planet in list
        planets[i].point = points[i]; // Assigns pre-existing point
        planets[i].num_connections = 0; // Set to 0
        planets[i].has_mixer = 0; // Set to no
    }

    // Create the connections
    for (i=0;i<num_points;i++){
        for (j=0;j<num_points;j++){
            if (j == i) continue; // kips the rest of the code if j == i
            double dist = get_distance(points[i], points[j]); // Find distance between planets
            if (dist < DIST_THRESHOLD){
                connections[planets[i].num_connections++] = &planets[j]; // ++ operation happens after value is accessed for assignment, assigns j'th planet in num_connections
            }
        }
        // Copy the connections to the planet
        planets[i].connections = malloc(sizeof(Planet*)*planets[i].num_connections);
        memcpy(planets[i].connections,
                connections,
                sizeof(Planet*)*planets[i].num_connections);
    }

    return planets;
}

Planet *find_by_id(Planet *planets, int num_planets, int id){ // This just returns a planet based on ID
    for (int i=0;i<num_planets;i++){
        if (planets[i].planet_id == id) return &planets[i];
    }
    return NULL;
}

int check_candidate(double distance, double max_distance){ // If distance is in a range based on max_distance, return 1
    if (distance > max_distance*0.6 && distance < max_distance*0.8) return 1;
    return 0;
}

/**
 * Selects the start planet and the treasure planet.
 */
void select_start_and_treasure(Planet *planets, int num_planets, Planet **start, Planet **treasure){
    double max_distance = 0;
    int i,j;

    for (i=0;i<num_planets;i++){ // Iterating through num_planets, incrementing i
        for (j=i+1;j<num_planets;j++){ // Iterating through num_planets, incrementing j
            double dist = get_distance(planets[i].point, planets[j].point); // Temp dist value between planet i and planet j
            if (dist > max_distance) max_distance = dist; // If temp dist is greater than the max distance, update the max distance
        }
    }

    printf("Max distance is %0.2f\n", max_distance); // Print max distance
    // Select a start node
    *start = &planets[rand()%num_planets]; // Generates random number between 0 and num_planets, &planets[] accesses address of planet with random ID

    // Find candidate treasure planets
    Planet *candidates[num_planets]; // Creates an array of pointers to planet objects, size of array is num_planets
    int num_candidates = 0;
    for (i=0;i<num_planets;i++){ // Iterating through the number of planets
        double dist = get_distance((*start)->point, planets[i].point); // Temp dist value is distance between the first planet and each other planet
        if (check_candidate(dist, max_distance)){ // If distance is in range
            candidates[num_candidates++] = &planets[i]; // Add that planet to candidates array, increments number of candidates
        }

    }

    // Select randomly from the candidates
    *treasure = candidates[rand()%num_candidates]; // Select random planet from candidates array
    (*treasure)->has_mixer = 1; // Set that as the planet with the particle mixer
}

Point* generate_poisson_points(double density, double xlim[2], double ylim[2], int *num_points) {
    double grid_size = 1.0;
    int num_cells_x = (int)ceil((xlim[1] - xlim[0]) / grid_size);
    int num_cells_y = (int)ceil((ylim[1] - ylim[0]) / grid_size);

    *num_points = 0; // Initializing the number of points
    for (int i = 0; i < num_cells_x; i++) { // For each X cell
        for (int j = 0; j < num_cells_y; j++) { // For each Y cell
            double expected_num_points = density * grid_size * grid_size; // Expected number of points
            int num_points_in_cell = (int)round(expected_num_points + sqrt(expected_num_points) * (drand48() * 2 - 1));
            // drand48() generates a random number between 0 and 1
            // 8" multiplies the number by 2
            // -1 lowers the range/number
            // Overall effect so far is generating random number between -1 and 1
            // Multiply this by the square root of the expected number of points
            // Add the expected, and round the answer
            *num_points += num_points_in_cell; // Add previous number to total number of points
        }
    }

    Point* points = (Point*)malloc(*num_points * sizeof(Point)); // Create an array of points
    if (points == NULL) { // Memory allocation check
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    int k = 0;
    for (int i = 0; i < num_cells_x; i++) { // For each X cell
        for (int j = 0; j < num_cells_y; j++) { // For each Y cell
            double x_base = i * grid_size;
            double y_base = j * grid_size;
            double expected_num_points = density * grid_size * grid_size;
            int num_points_in_cell = (int)round(expected_num_points + sqrt(expected_num_points) * (drand48() * 2 - 1)); // Calculating the number of points for the cell
            for (int l = 0; l < num_points_in_cell; l++) { // Creating points in cell
                double x = x_base + drand48() * grid_size; // Random X position
                double y = y_base + drand48() * grid_size; // Random Y position
                if (x <= xlim[1] && y <= ylim[1]) { // If the generated point is inside the limit, set the point
                    points[k].x = x;
                    points[k].y = y;
                    k++; // Record creation of point
                }
                if (k == *num_points) { // If you have enough points, finish and return the points list
                    return points;
                }
            }
        }
    }

    return points;
}

int main(int argc, char *argv[]) {
    // Define parameters
    double density = 3;  // Adjust density as needed
    double xlim[2] = {0, 10};  // X-axis limits
    double ylim[2] = {0, 10};  // Y-axis limits
    int num_points;

    // Seed for PRNG
    long int seed;
    if (argc > 1) {
        seed = strtol(argv[1], NULL, 10);
    } else {
        seed = 12;  // Default seed if not provided
        // Default: 12
        // Doesn't run: 1, 5, 8
        // Good seeds: 22
    }
    srand48(seed);

    // Generate Poisson points
    Point* points = generate_poisson_points(density, xlim, ylim, &num_points);
    printf("Generated universe\n");

    // Convert to planets
    Planet *planets = points_to_planets(points, num_points);
    // Select the start and treasure
    Planet *start, *treasure;
    select_start_and_treasure(planets, num_points, &start, &treasure);
    printf("Selected start point\n");
    // Run the game
    void *ship_state = NULL;
    int num_hops = 0;  // limit number of steps to number of planets
    Planet *crt = start;
    int found_treasure = 0;
    while (num_hops < num_points){
        num_hops++;
        unsigned int crt_id = crt->planet_id;
        unsigned int *connections;
        connections = malloc(sizeof(unsigned int)*crt->num_connections);
        for (int i=0;i<crt->num_connections;i++){
            connections[i] = crt->connections[i]->planet_id;
            printf("crt_id: %d\n", crt_id);
            printf("crt-connections[i]->planet_id: %d\n", crt->connections[i]->planet_id);
        }
        double crt_dist = get_distance(crt->point, treasure->point);
        ShipAction next_action = space_hop(crt_id,
                                        connections,
                                        crt->num_connections,
                                        crt_dist,
                                        ship_state);
        ship_state = next_action.ship_state;
        unsigned int next_planet = next_action.next_planet;
        free(connections);
        // Find next planet
        if (next_planet == RAND_PLANET){
            crt = &planets[rand()%num_points];
        }else{
            // Look first in the connections of the current
            int found = 0;
            for (int i=0;i<crt->num_connections;i++){
                if (crt->connections[i]->planet_id == next_planet){
                    crt = crt->connections[i];
                    found = 1;
                    break;
                }
            }
            if (!found){
                // Look at all the planets
                Planet *next = find_by_id(planets, num_points, next_planet);
                if (next != NULL){
                    crt = next;
                }
            }
        }
        if (crt->has_mixer){
            found_treasure = 1;
            break;
        }
        printf("\n"); // My own debugging stuff :3 :3 :3 :3 :3 :3 :3 :3 :3 :3 :3 :3 :3 :3 :3 :3 :3
    }

    if (found_treasure){
        printf("Congratulations you found it in %d hops!\n", num_hops);
    }else{
        printf("The hyper-engine dies, you're lost in space. Ghost riders in the sky...\n");
    }

    // Free allocated memory
    for (int i=0;i<num_points;i++){
        free(planets[i].connections);
    }
    free(planets);
    free(points);

    return num_hops;
}
