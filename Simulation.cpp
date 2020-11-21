#ifndef SIMULATION
#define SIMULATION

#include "v2.cpp"
#include <vector>
#include <cstdlib>
#include <stdio.h>
#include <thread>
#include <math.h>

/*
   Handle collisions. Can start out by assuming that all collisions are perfectly inelastic (particles combine).
   If dist < threshold, sum masses, maybe velocities too for jokes.
   Branch before those changes though. Then, can make an algorithm to judge order, and try different equations (1st, just different exponents of dist) 
*/

using namespace std;

class Simulation {
public:

    struct particle {
        v2 pos;
        float mass;
        v2 vel;
    };

    // use a double-buffering technique
    vector<particle>* particles;
    vector<particle>* newParticles;

    float normRand() {
        return rand() / (float) RAND_MAX;
    }

    Simulation(int numParticles, int width, int height) {
        particles = new vector<particle>(numParticles);
        newParticles = new vector<particle>(numParticles);

        float r = 100;
        for (int i = 0; i < numParticles; ++i) {
            float angle = i / (float) numParticles * 2 * M_PI;
            float x = width / 2 + r * cos(angle);
            float y = height / 2 + r * sin(angle);
            particles->at(i) = {{x, y}, 1.0f, {0.0f, 0.0f}};
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

    void concurrentStep(float timestep) {
        int size = particles->size();
        vector<thread*> simThreads(size);

        // create a thread to handle each particle's simulation
        for (int i = 0; i < size; ++i) {
            simThreads[i] = new thread(&Simulation::stepParticle, this, timestep, i);
        }

        // wait for all of the threads
        for (int i = 0; i < size; ++i) {
            thread* t = simThreads[i]; 
            t->join();
            delete t;
        }

        // swap particles and new particles ref
        vector<particle>* temp = particles;
        particles = newParticles;
        newParticles = temp;
    }

    // TODO: thread-safe vector? accessing same mem at once, not modifying it tho
    void stepParticle(float timestep, int particleIndex) {
        particle p = particles->at(particleIndex);

        // compute pairwise forces
        v2 resultant = {0.0f, 0.0f};
        int size = particles->size();
        for (int j = 0; j < size; ++j) {
            if (j == particleIndex) continue;
            particle other = particles->at(j);
            v2 diff = sub(other.pos, p.pos);
            float mag = getAttraction(p, other);
            //printf("mag: %f\n", mag);
            diff = scalarMult(diff, mag);
            resultant = add(resultant, diff);
        }
        //printf("resultant: %s\n", v2Str(resultant).c_str());

        // use force to get acceleration
        v2 accel = scalarMult(resultant, 1.0f / p.mass);

        // apply velocity to position, then apply accel to velocity
        v2 newPos = add(p.pos, scalarMult(p.vel, timestep));
        v2 newVel = add(p.vel, scalarMult(accel, timestep));
        particle newP = {newPos, p.mass, newVel};

        newParticles->at(particleIndex) = newP;            
    }

    float getAttraction(particle pa, particle pb) {
        v2 a = pa.pos;
        v2 b = pb.pos;
        float at = 100 / distSq(a, b);
        return at;
    }
};

#endif
