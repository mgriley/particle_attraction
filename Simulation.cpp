#ifndef SIMULATION
#define SIMULATION

#include "v2.cpp"
#include <vector>
#include <cstdlib>
#include <stdio.h>

using namespace std;

class Simulation {
public:

    struct particle {
        v2 pos;
        float mass;
        v2 vel;
    };

    vector<particle>* particles;

    float normRand() {
        return rand() / (float) RAND_MAX;
    }

    Simulation(int numParticles, int width, int height) {
        printf("num particles: %d", numParticles);
        particles = new vector<particle>(numParticles);
        for (int i = 0; i < numParticles; ++i) {
            float randX = width * normRand();
            float randY = height * normRand();
            particles->at(i) = {{randX, randY}, 1.0f, {0.0f, 0.0f}};
        }
        printf("size: %d", (int) particles->size()); 
    }

    ~Simulation() {
        delete particles;
    }
    
    void step(float timestep) {

        // sum the attractions for each particle
        // compute pairwise forces O(n^2)    
        // TODO: only compute the attraction once per pair 
        int size = particles->size();
        vector<particle>* newParticles = new vector<particle>(size);
        for (int i = 0; i < size; ++i) {
            particle p = particles->at(i);

            // compute pairwise forces
            v2 resultant = {0.0f, 0.0f};
            for (int j = 0; j < size; ++j) {
                if (j == i) continue;
                particle other = particles->at(j);
                v2 diff = sub(other.pos, p.pos);
                float mag = getAttraction(p, other);
                //printf("mag: %f\n", mag);
                diff = scalarMult(diff, mag);
                resultant = add(resultant, diff);
            }
            printf("resultant: %s\n", v2Str(resultant).c_str());

            // use force to get acceleration
            v2 accel = scalarMult(resultant, 1.0f / p.mass);

            // apply velocity to position, then apply accel to velocity
            v2 newPos = add(p.pos, scalarMult(p.vel, timestep));
            v2 newVel = add(p.vel, scalarMult(accel, timestep));
            particle newP = {newPos, p.mass, newVel};
            newParticles->at(i) = newP;
        }
        // swap particles with newParticles
        delete particles;
        particles = newParticles;
    }

    float getAttraction(particle a, particle b) {
        float dist_sq = pow(a.pos.x - b.pos.x, 2) + pow(a.pos.y - b.pos.y, 2);
        return 1000.0 / (dist_sq == 0 ? 0.001 : dist_sq);
    }
};

#endif
