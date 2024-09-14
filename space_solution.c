#include "space_explorer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct planet_struct {
    unsigned int Planet_ID;
    double Planet_Distance;
    int Number_Of_Connections;
    unsigned int *connections;
    int neighborsExplored;
};

int hopCounter = 0;
int First_Run = 1;
struct planet_struct *planetArray = NULL;
int numberOfPlanets = 0;
int planetAlreadyExists = 0;
int exploringNeighbors = 0;
struct planet_struct planetWithNeighbors;
int neighborIterator = 0;



ShipAction space_hop(unsigned int crt_planet,
                     unsigned int *connections,
                     int num_connections,
                     double distance_from_mixer,
                     void *ship_state)
{
    /* your solution here */

    for (int i = 0; i < 5; i++) {
        printf("\n");
    }

    printf("////////////////////////////////\n");
    printf("// Start Of Space Hop Function\n");
    printf("////////////////////////////////\n");
    printf("\n");

    ShipAction action;
    hopCounter++;

    if (First_Run) { // Do a random hop for the first one
        First_Run = 0;
        printf("First run of space_hop function. Returning random planet. \n");
        action.next_planet = RAND_PLANET;
        printf("\n");
        printf("Current planet: %u", crt_planet);
        printf("Next planet: %u", action.next_planet);
        printf("\n");
        printf("////////////////////////////////\n");
        printf("// End Of Space Hop Function (1)\n");
        printf("////////////////////////////////\n");
        for (int i = 0; i < 5; i++) {
            printf("\n");
        }
        return action;
    }

    // We discard the very first planet initially given to us, jump to a random planet, and then start applying the logic

    if (hopCounter % 5 == 0) { // Every 5 hops, do a random hop
        action.next_planet = RAND_PLANET;
        printf("\n");
        printf("Current planet: %u", crt_planet);
        printf("Next planet: %u", action.next_planet);
        printf("\n");
        printf("////////////////////////////////\n");
        printf("// End Of Space Hop Function (2)\n");
        printf("////////////////////////////////\n");
        for (int i = 0; i < 5; i++) {
            printf("\n");
        }
        return action;
    }



    ////////////////////////////////
    // Receiving A New Planet
    ////////////////////////////////

    if (numberOfPlanets == 0) { // If first
        printf("First planet: %u\n", crt_planet);
        struct planet_struct newPlanet; // Create struct of new planet
        newPlanet.Planet_ID = crt_planet;
        newPlanet.Planet_Distance = distance_from_mixer;
        newPlanet.Number_Of_Connections = num_connections;
        newPlanet.connections = connections;
        newPlanet.neighborsExplored = 0;

        planetArray = malloc((numberOfPlanets + 1) + (sizeof(struct planet_struct))); // Adjusting memory allocation for planetArray

        planetArray[0] = newPlanet;
        numberOfPlanets++;
    }
    else {
        printf("Number of planets is not 0\n");
        printf("Number of planets: %d\n", numberOfPlanets);
        for (int i = 0; i < numberOfPlanets; i++) { // For each existing planet
            if (planetArray[i].Planet_ID == crt_planet) { // If the planet is already in the array
                printf("Planet %u has already been visited. \n", planetArray[i].Planet_ID);
                planetAlreadyExists = 1;
                break;
            }
        }
        if (planetAlreadyExists == 0) { // If the planet isn't already in the array
            printf("The planet does not already exist. \n");
            struct planet_struct newPlanet;
            newPlanet.Planet_ID = crt_planet;
            newPlanet.Planet_Distance = distance_from_mixer;
            newPlanet.Number_Of_Connections = num_connections;
            newPlanet.connections = connections;
            newPlanet.neighborsExplored = 0;

            planetArray = realloc(planetArray, (numberOfPlanets + 1) * (sizeof(struct planet_struct))); // Adjusting memory allocation for planetArray

            planetArray[numberOfPlanets] = newPlanet; // Add new planet to array
            printf("Assigning new planet to array. \n");
            numberOfPlanets++;
        }
        planetAlreadyExists = 0;
    }



    ////////////////////////////////
    // Deciding Which Planet To Go To Next
    ////////////////////////////////

    if (exploringNeighbors == 1) { // If we are currently exploring the neighbors of a planet
        printf("We are exploring neighbors. \n");
        if (neighborIterator < planetWithNeighbors.Number_Of_Connections) { // If we still have neighbors left to explore

            printf("We still have neighbors left to explore. \n");
            for (int i = 0; i < numberOfPlanets; i++) {
                if (planetWithNeighbors.connections[neighborIterator] == planetArray[i].Planet_ID){ // If we've already visited this one, skip to the next to preserve hops
                    printf("We have already visited this planet. \n");
                    neighborIterator++;
                    break;
                }
            }
            action.next_planet = planetWithNeighbors.connections[neighborIterator]; // Set the next neighbor as the next planet
            printf("Returning next neighbor. \n");
            neighborIterator++;
        }
        else {
            exploringNeighbors = 0; // If there are no more neighbors to explore, reset the values
            neighborIterator = 0;
            planetWithNeighbors.neighborsExplored = 1; // The neighbors of this planet have been explored
            printf("We have explored all of this planet's neighbors. \n");
        }

    }
    if (exploringNeighbors == 0) { // If we are not currently exploring neighbors of a specific planet
        printf("We are not currently exploring neighbors. \n");
        struct planet_struct closestPlanet; // Find the closest of the planets we have but have not explored the neighbors of
        double shortestDistance = 100; // Default large distance initialized for comparisons
        int closestPlanetFound = 0; // We have not yet found the closest
        for (int i = 0; i < numberOfPlanets; i++) { // For each planet
            printf("Checking planet with ID: %u\n", planetArray[i].Planet_ID);
            if (planetArray[i].Planet_Distance < shortestDistance && planetArray[i].neighborsExplored == 0) { // If its distance is less than the shortest and its neighbors have not been explored
                printf("The neighbours of this planet have not yet been explored, and it's distance is %f\n", planetArray[i].Planet_Distance);
                shortestDistance = planetArray[i].Planet_Distance; // Set new shortest distance
                closestPlanet = planetArray[i]; // Set it as closest
                closestPlanetFound = 1; // We have found the closest neighbor
                action.next_planet = closestPlanet.Planet_ID;
                exploringNeighbors = 1;
                planetWithNeighbors = planetArray[i];
            }
        }
        if (closestPlanetFound == 0) { // If there was no neighbor whose neighbors were not already explored
            printf("A closest planet has not been found, so we will do a random jump. \n");
            action.next_planet = RAND_PLANET; // Jump to a random planet
        }
    }



    ////////////////////////////////
    // End Of Space Hop Function
    ////////////////////////////////

    printf("\n");
    printf("Current planet: %u", crt_planet);
    printf("Next planet: %u", action.next_planet);
    printf("\n");
    printf("////////////////////////////////\n");
    printf("// End Of Space Hop Function (3)\n");
    printf("////////////////////////////////\n");
    for (int i = 0; i < 5; i++) {
        printf("\n");
    }

    action.ship_state = ship_state;

    return action;

}

/**
 * Take a planet
 * Create a struct for the planet
 * Struct has planet values (ID, distance, neighbours, etc),
 * A visited_planets array will store a struct of every planet that has been visited
 * Check the neighbours of the current planet, do every one. Store them, and then search the neighbours of the closest.
 *
 * When neighbors are explored, structs are created, they already exist, can their neighbors be explored?
 **/
