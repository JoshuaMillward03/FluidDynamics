#include "raylib.h"
#include "raymath.h"
#include <algorithm>
#include <vector>

class Node{
    private:
        inline static int nextId = 0;
        int id;
        int radius = 5;
        float mass;
        float damping = 0.7f;
        float density = 0.0f;
        float viscosityStrength = 0.5f;
        const float attractionZone = 1.5f; 
        const float smoothingRadius = 40.0f;
        float pressure = 0.0f;
        float pressureMultiplier = 400.0f;
        Vector2 position;
        Vector2 velocity;
        Vector2 acceleration;
        Color color = BLUE;

        const float Gravity = 600.0f;
    public:
        Node(Vector2 startPos,  float nodeMass){
            id = nextId;
            nextId += 1;
            position = startPos;
            mass = nodeMass;
            velocity = {0.0f, 0.0f};
            acceleration = {0.0f, 0.0f};
        };

        Vector2 getPosition() const { return position; }
        Vector2 getVelocity() const { return velocity; }
        Vector2 getAcceleration() const { return acceleration; }
        float getDensity() const { return density; }
        float getPressure() const { return pressure; }
        void setPosition(Vector2 pos) { position = pos; }
        void setVelocity(Vector2 vel) { velocity = vel; }
        void setAcceleration(Vector2 acc) { acceleration = acc; }
        void setDensity( float d ) { density = d; }
        void setPressure( float p ) { pressure = p; }

        void drawNode(){
            DrawCircleV(position, radius, color);
        }

        void resolveBorderCollisions(int screenWidth, int screenHeight){
            if (position.y + radius > screenHeight){
                position.y = screenHeight - radius;
                velocity.y *= -1 * damping;
            }
            if (position.y - radius < 0){
                position.y = radius;
                velocity.y *= -1 * damping;
            }
            if (position.x + radius > screenWidth){
                position.x = screenWidth - radius;
                velocity.x *= -1 * damping;
            }
            if (position.x - radius < 0){
                position.x = radius;
                velocity.x *= -1 * damping;
            }
        }

        void calculateDensity(const std::vector<Node>& nodes){
            density = 0.0f;
            float smoothingRadiusSqr = smoothingRadius * smoothingRadius;
            for (const auto& comparingNode : nodes){
                float distanceSqr = Vector2DistanceSqr(position, comparingNode.position);

                if (distanceSqr < smoothingRadiusSqr){
                    float influence = 1.0f - distanceSqr / smoothingRadiusSqr;
                    density += influence * influence; // Smoothing Kernal: (1 - x^2)^2
                }
            }

            pressure = density * pressureMultiplier;
        }

        void calculateForces(const std::vector<Node>& nodes, int screenWidth, int screenHeight){
            //Node to node forces
            for (auto& otherNode : nodes){
                if(id == otherNode.id){
                    continue;
                }
                Vector2 direction = otherNode.position - position;
                float distance = Vector2Length(direction);

                if (distance > 0 && distance < smoothingRadius){
                    Vector2 normalizedDirection = Vector2Scale(direction, 1.0f / distance);
                    float x = distance / smoothingRadius;

                    //influence = (1 - x)^2 * (1 - k * x)
                    float influence = (1.0f - x) * (1.0f - x) * (1.0f - attractionZone * x);

                    // Surface Tension
                    if (influence < 0.0f) {
                        float attractionStrength = 5.0f;
                        influence *= attractionStrength; 
                    }

                    //Pressure force
                    float sharedPressure = (pressure + otherNode.getPressure()) / 2.0f;
                    float pressureForceMagnitude = sharedPressure * influence;
                    Vector2 pressureForce = normalizedDirection * -pressureForceMagnitude;

                    acceleration += pressureForce;

                    //Viscocity
                    float viscoInfluence = 1.0f - x; 
                    Vector2 relativeVel = otherNode.velocity - velocity;
                    Vector2 viscosityForce = relativeVel * (viscoInfluence * viscosityStrength);
                    acceleration += viscosityForce;
                }
            }
            // // Boundary wall test
            // float boundaryForceStrength = 2000.0f; 

            // // Left Wall
            // if (position.x < smoothingRadius) {
            //     float x = position.x / smoothingRadius;
            //     acceleration.x += (1.0f - x) * (1.0f - x) * boundaryForceStrength;
            // }
            // // Right Wall
            // if (position.x > screenWidth - smoothingRadius) {
            //     float distance = screenWidth - position.x;
            //     float x = distance / smoothingRadius;
            //     acceleration.x -= (1.0f - x) * (1.0f - x) * boundaryForceStrength;
            // }
            // // Top Wall
            // if (position.y < smoothingRadius) {
            //     float x = position.y / smoothingRadius;
            //     acceleration.y += (1.0f - x) * (1.0f - x) * boundaryForceStrength;
            // }
            // // Bottom Wall
            // if (position.y > screenHeight - smoothingRadius) {
            //     float distance = screenHeight - position.y;
            //     float x = distance / smoothingRadius;
            //     acceleration.y -= (1.0f - x) * (1.0f - x) * boundaryForceStrength;
            // }
            
        }

        void updateNode(float deltaTime){
            acceleration.y += Gravity;
            velocity += acceleration * deltaTime;
            velocity *= 0.995f;
            position += velocity * deltaTime;
            acceleration = { 0.0f, 0.0f };
        }
};

class ParticleSystem{
    private:
        std::vector<Node> nodes;
    public:
        void updateAll(float deltaTime){
            int screenHeight = GetScreenHeight();
            int screenWidth = GetScreenWidth();

            for (auto& node : nodes){
                node.calculateDensity(nodes);
            }

            // calculate forces from Pressure and Viscosity. 
            for (auto& node : nodes){
                node.calculateForces(nodes, screenWidth, screenHeight);
            }

            //Update position, calculate colisions, and draw nodes
            for (auto& node : nodes){
                node.updateNode(deltaTime);
                node.resolveBorderCollisions(screenWidth, screenHeight);
            }
        };

        void drawAll(){
            for (auto& node : nodes){
                node.drawNode();
            }
        };

        void addNode(Vector2 position, float mass){
            nodes.emplace_back(position, mass);
        }

        void addBlock(Vector2 position, float mass, int width, int height, int nodeSpacing){
            for (size_t j = 0; j < height; j++){
                for (size_t i = 0; i < width; i++){
                    float spawnX = position.x + (i * nodeSpacing);
                    float spawnY = position.y + (j * nodeSpacing);
                    
                    addNode({spawnX, spawnY}, mass);
                }
            }
        }
};

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 800;

    ParticleSystem nodes;
    nodes.addBlock({25.0f, 400.0f}, 1.0f, 50, 20, 15);

    InitWindow(screenWidth, screenHeight, "Raylib Fluid Test");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        nodes.updateAll(deltaTime);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        nodes.drawAll();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}