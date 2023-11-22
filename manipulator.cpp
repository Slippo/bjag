#include "manipulator.h"
#include <iostream>

namespace game {

    Manipulator::Manipulator() {}
    Manipulator::~Manipulator() {}

    // (1) Construct hierarchical objects
    CompositeNode* Manipulator::ConstructKelp(ResourceManager* resman_, std::string name_, int branch_complexity, glm::vec3 position_) {

        CompositeNode* kelp = new CompositeNode(name_);
        kelp->SetType(CompositeNode::Type::Kelp);

        // Create root node
        SceneNode* root = CreateSceneNodeInstance("Root", "Cylinder", "KelpMaterial", "", resman_);
        root->SetPosition(position_);
        root->SetPivot(glm::vec3(0, -1, 0));
        std::cout << glm::to_string(root->GetPivot());
        root->SetType(SceneNode::Type::KelpStem);
        kelp->SetRoot(root);

        // Create branches and sub-branches   
        float theta = 2 * glm::pi<float>();
        float offset = 0.2f;
        float stem_len = 2.0f;
        // CREATE BRANCHES CONNECTED TO THE ROOT NODE
        for (int i = 0; i < branch_complexity + 1; i++) {
            float frac = float(i) / (branch_complexity + 1);
            SceneNode* branch = CreateSceneNodeInstance("Branch", "Cylinder", "KelpMaterial", "", resman_);
            branch->SetType(SceneNode::Type::KelpStem); // type specified for shader
            branch->SetPosition(glm::vec3(0, 2, 0)); // 2 units above the root
            branch->SetPivot(glm::vec3(0, -stem_len / 2, 0));
            // Orbit about the pivot to set the starting orientation
            branch->Orbit(glm::angleAxis(theta / 12, glm::vec3(cos(theta * frac), 0, sin(theta * frac))));
            root->AddChild(branch);
            kelp->AddNode(branch);

            // CREATE LEAVES FOR THE CURRENT BRANCH
            // THE BRANCH HAS LOWER LEAVES AND HIGH LEAVES

            for (int j = 0; j < branch_complexity; j++) {
                float theta = 2.0f * glm::pi<float>();
                float frac = float(j) / (branch_complexity);
                SceneNode* high_leaf = CreateSceneNodeInstance("Leaf", "Sphere", "KelpMaterial", "", resman_);
                high_leaf->SetType(SceneNode::Type::KelpLeaf);
                high_leaf->SetScale(0.25f * glm::vec3(1.0f + abs(cos(theta * frac)), 0.2f, 1.0f + abs(sin(theta * frac))));
                high_leaf->Scale(glm::vec3(0.75f));
                high_leaf->SetPosition(glm::vec3(offset * cos(theta * frac), 1, offset * sin(theta * frac)));
                high_leaf->SetPivot(glm::vec3(0, -1, 0));
                SceneNode* low_leaf = CreateSceneNodeInstance("SubSubBranch", "Sphere", "KelpMaterial", "", resman_);
                low_leaf->SetType(SceneNode::Type::KelpLeaf);
                low_leaf->SetScale(0.25f * glm::vec3(1.0f + abs(cos(theta * frac)), 0.2f, 1.0f + abs(sin(theta * frac))));
                low_leaf->SetPosition(glm::vec3(offset * cos(theta * frac), 0, offset * sin(theta * frac)));
                low_leaf->SetPivot(glm::vec3(0, -stem_len / 2, 0));
                low_leaf->Scale(glm::vec3(0.5f));

                branch->AddChild(low_leaf);
                kelp->AddNode(low_leaf);
                branch->AddChild(high_leaf);
                kelp->AddNode(high_leaf);
            }
            // CREATE SUB BRANCHES FOR CURRENT BRANCH
            for (int k = 0; k < branch_complexity; k++) {
                float frac = float(k) / branch_complexity;
                SceneNode* sub_branch = CreateSceneNodeInstance("SubBranch", "Cylinder", "KelpMaterial", "", resman_);
                sub_branch->SetType(SceneNode::Type::KelpStem);
                sub_branch->SetPosition(glm::vec3(0, stem_len, 0));
                sub_branch->SetPivot(glm::vec3(0, -stem_len / 2, 0));
                sub_branch->Orbit(glm::angleAxis(-theta / 16.0f, glm::vec3(cos(theta * frac), 0, sin(theta * frac))));
                branch->AddChild(sub_branch);
                kelp->AddNode(sub_branch);

                // CREATE LEAVES FOR CURRENT SUB BRANCH
                for (int j = 0; j < branch_complexity; j++) {
                    float theta = 2.0f * glm::pi<float>();
                    float frac = float(j) / (branch_complexity);
                    SceneNode* leaf = CreateSceneNodeInstance("Leaf", "Sphere", "KelpMaterial", "", resman_);
                    leaf->SetType(SceneNode::Type::KelpLeaf);
                    leaf->SetScale(0.25f * glm::vec3(1.0f + abs(cos(theta * frac)), 0.2f, 1.0f + abs(sin(theta * frac))));
                    leaf->SetPosition(glm::vec3(offset * cos(theta * frac), 1, offset * sin(theta * frac)));
                    leaf->SetPivot(glm::vec3(0, -stem_len / 2, 0));
                    sub_branch->AddChild(leaf);
                    kelp->AddNode(leaf);
                }
            }
        }
        return kelp;
    }


    CompositeNode* Manipulator::ConstructPlane(ResourceManager* resman_) {
        CompositeNode* plane = new CompositeNode("Plane");

        SceneNode* root = CreateSceneNodeInstance("Root", "Plane", "NormalMapMaterial", "NormalMapSand", resman_);
        root->SetColor(glm::vec3(1, 0.9, 0.5));
        plane->SetRoot(root);

        return plane;
    }

    CompositeNode* Manipulator::ConstructBoundary(ResourceManager* resman_) {
        CompositeNode* boundary = new CompositeNode("Boundary");

        SceneNode* root = CreateSceneNodeInstance("Root", "Boundary", "NormalMapMaterial", "NormalMapStone", resman_);
        root->SetColor(glm::vec3(0.6, 0.6, 0.7));
        boundary->SetRoot(root);

        return boundary;
    }

    CompositeNode* Manipulator::ConstructSun(ResourceManager* resman, glm::vec3 position) {
        CompositeNode* sphere = new CompositeNode("Sphere");

        SceneNode* root = CreateSceneNodeInstance("Root", "Sphere", "NormalMapMaterial", "", resman);
        root->SetPosition(position);
        sphere->SetRoot(root);

        return sphere;
    }

    CompositeNode* Manipulator::ConstructStalagmite(ResourceManager* resman_, std::string name_, glm::vec3 position_) {
        CompositeNode* stalagmite = new CompositeNode(name_);
        stalagmite->SetType(CompositeNode::Type::Stalagmite);

        // Create root node
        SceneNode* root = CreateSceneNodeInstance("Root", "StalagmiteBase", "KelpMaterial", "", resman_);
        //root->SetScale(glm::vec3(1.0, 1.0, 1.0));
        root->SetPosition(position_);
        stalagmite->SetRoot(root);

        float s = 0.9;
        float h = 0.0;
        for (int i = 0; i < 8; i++) {
            // Base (graudally gets taller & thinner)
            SceneNode* part = CreateSceneNodeInstance("Base", "StalagmiteBase", "KelpMaterial", "", resman_);
            part->Scale(glm::vec3(s, i * s, s));
            part->Translate(glm::vec3(0, h, 0));
            stalagmite->AddNode(part);
            s -= 0.1;
            h += 2.0;

            // Spikes (one on each side)
            SceneNode* spike = CreateSceneNodeInstance("Base", "StalagmiteSpike", "KelpMaterial", "", resman_);
            spike->Scale(glm::vec3(1.0, 2.0, 1.0));
            spike->SetPosition(part->GetPosition() + glm::vec3(part->GetScale().y, 0, 0));
            spike->Rotate(glm::angleAxis(glm::half_pi<float>(), glm::vec3(0, 0, -1)));
            stalagmite->AddNode(spike);

            spike = CreateSceneNodeInstance("Base", "StalagmiteSpike", "KelpMaterial", "", resman_);
            spike->Scale(glm::vec3(1.0, 2.0, 1.0));
            spike->SetPosition(part->GetPosition() + glm::vec3(-(part->GetScale().y), 0, 0));
            spike->Rotate(glm::angleAxis(glm::half_pi<float>(), glm::vec3(0, 0, 1)));
            stalagmite->AddNode(spike);

            spike = CreateSceneNodeInstance("Base", "StalagmiteSpike", "KelpMaterial", "", resman_);
            spike->Scale(glm::vec3(1.0, 2.0, 1.0));
            spike->SetPosition(part->GetPosition() + glm::vec3(0, 0, part->GetScale().y));
            spike->Rotate(glm::angleAxis(glm::half_pi<float>(), glm::vec3(1, 0, 0)));
            stalagmite->AddNode(spike);

            spike = CreateSceneNodeInstance("Base", "StalagmiteSpike", "KelpMaterial", "", resman_);
            spike->Scale(glm::vec3(1.0, 2.0, 1.0));
            spike->SetPosition(part->GetPosition() + glm::vec3(0, 0, -(part->GetScale().y)));
            spike->Rotate(glm::angleAxis(glm::half_pi<float>(), glm::vec3(-1, 0, 0)));
            stalagmite->AddNode(spike);
        }

        SceneNode* tip = CreateSceneNodeInstance("Base", "StalagmiteSpike", "KelpMaterial", "", resman_);
        tip->SetScale(glm::vec3(1.0, 2.0, 1.0));
        tip->Translate(glm::vec3(0, 17.0, 0));
        stalagmite->AddNode(tip);

        return stalagmite;
    }

    CompositeNode* Manipulator::ConstructSubmarine(ResourceManager* resman_, std::string name_, glm::vec3 position_) {
        CompositeNode* Submarine = new CompositeNode(name_);
        Submarine->SetType(CompositeNode::Type::Submarine);

        // Base of submarine
        SceneNode* root = CreateSceneNodeInstance("Root", "SubmarineBase", "ObjectMaterial", "", resman_);
        root->SetColor(glm::vec3(1.0, 0.9, 0.3));
        root->SetScale(glm::vec3(1.2, 0.5, 0.7));
        root->SetPosition(position_);
        Submarine->SetRoot(root);

        // Front window
        SceneNode* front_window = CreateSceneNodeInstance("FrontWindow", "SubmarineBase", "ObjectMaterial", "", resman_);
        front_window->SetColor(glm::vec3(0.6, 0.8, 1.0));
        front_window->Scale(glm::vec3(0.5, 0.5, 0.6));
        front_window->Translate(glm::vec3(-5.7, 0, 0));
        front_window->Rotate(glm::angleAxis(glm::pi<float>(), glm::vec3(0, 0, 1)));
        Submarine->AddNode(front_window);

        // Side windows
        int flip = 1;
        for (int i = 0; i < 2; i++) {
            int x = 0;
            for (int j = 0; j < 4; j++) {
                SceneNode* side_window = CreateSceneNodeInstance("SideWindow", "SubmarineBase", "ObjectMaterial", "", resman_);
                side_window->SetColor(glm::vec3(0.6, 0.8, 1.0));
                side_window->Scale(glm::vec3(0.1, 0.2, 0.1));
                float z = 9.5;
                if (j == 0 || j == 3) { z = 8.5; }
                side_window->Translate(glm::vec3(flip * (x - 5), 0, flip * z));
                Submarine->AddNode(side_window);
                x += 3;
            }
            flip = -1;
        }

        // Legs
        SceneNode* leg1 = CreateSceneNodeInstance("Leg", "Cylinder", "ObjectMaterial", "", resman_);
        leg1->SetColor(glm::vec3(0.4, 0.4, 0.4));
        leg1->Scale(glm::vec3(2.0, 4.0, 2.0));
        leg1->Translate(glm::vec3(5, -7, 5));
        leg1->Rotate(glm::angleAxis(glm::quarter_pi<float>(), glm::vec3(-1, 0, 0)));
        Submarine->AddNode(leg1);
        SceneNode* leg1_a = CreateSceneNodeInstance("Leg", "Cylinder", "ObjectMaterial", "", resman_);
        leg1_a->SetColor(glm::vec3(0.4, 0.4, 0.4));
        leg1_a->Scale(glm::vec3(1, 1, 1));
        leg1_a->Translate(glm::vec3(0, -1.3, -0.9));
        leg1_a->Rotate(glm::angleAxis(glm::quarter_pi<float>(), glm::vec3(1.5, 0, 0)));
        leg1->AddChild(leg1_a);
        Submarine->AddNode(leg1_a);

        SceneNode* leg2 = CreateSceneNodeInstance("Leg", "Cylinder", "ObjectMaterial", "", resman_);
        leg2->SetColor(glm::vec3(0.4, 0.4, 0.4));
        leg2->Scale(glm::vec3(2.0, 4.0, 2.0));
        leg2->Translate(glm::vec3(5, -7, -5));
        leg2->Rotate(glm::angleAxis(glm::quarter_pi<float>(), glm::vec3(1, 0, 0)));
        Submarine->AddNode(leg2);
        SceneNode* leg2_a = CreateSceneNodeInstance("Leg", "Cylinder", "ObjectMaterial", "", resman_);
        leg2_a->SetColor(glm::vec3(0.4, 0.4, 0.4));
        leg2_a->Scale(glm::vec3(1, 1, 1));
        leg2_a->Translate(glm::vec3(0, -1.3, 0.9));
        leg2_a->Rotate(glm::angleAxis(glm::quarter_pi<float>(), glm::vec3(-1.5, 0, 0)));
        leg2->AddChild(leg2_a);
        Submarine->AddNode(leg2_a);

        SceneNode* leg3 = CreateSceneNodeInstance("Leg", "Cylinder", "ObjectMaterial", "", resman_);
        leg3->SetColor(glm::vec3(0.4, 0.4, 0.4));
        leg3->Scale(glm::vec3(2.0, 4.0, 2.0));
        leg3->Translate(glm::vec3(-5, -7, 5));
        leg3->Rotate(glm::angleAxis(glm::quarter_pi<float>(), glm::vec3(-1, 0, 0)));
        Submarine->AddNode(leg3);
        SceneNode* leg3_a = CreateSceneNodeInstance("Leg", "Cylinder", "ObjectMaterial", "", resman_);
        leg3_a->SetColor(glm::vec3(0.4, 0.4, 0.4));
        leg3_a->Scale(glm::vec3(1, 1, 1));
        leg3_a->Translate(glm::vec3(0, -1.3, -0.9));
        leg3_a->Rotate(glm::angleAxis(glm::quarter_pi<float>(), glm::vec3(1.5, 0, 0)));
        leg3->AddChild(leg3_a);
        Submarine->AddNode(leg3_a);

        SceneNode* leg4 = CreateSceneNodeInstance("Leg", "Cylinder", "ObjectMaterial", "", resman_);
        leg4->SetColor(glm::vec3(0.4, 0.4, 0.4));
        leg4->Scale(glm::vec3(2.0, 4.0, 2.0));
        leg4->Translate(glm::vec3(-5, -7, -5));
        leg4->Rotate(glm::angleAxis(glm::quarter_pi<float>(), glm::vec3(1, 0, 0)));
        Submarine->AddNode(leg4);
        SceneNode* leg4_a = CreateSceneNodeInstance("Leg", "Cylinder", "ObjectMaterial", "", resman_);
        leg4_a->SetColor(glm::vec3(0.4, 0.4, 0.4));
        leg4_a->Scale(glm::vec3(1, 1, 1));
        leg4_a->Translate(glm::vec3(0, -1.3, 0.9));
        leg4_a->Rotate(glm::angleAxis(glm::quarter_pi<float>(), glm::vec3(-1.5, 0, 0)));
        leg4->AddChild(leg4_a);
        Submarine->AddNode(leg4_a);

        return Submarine;
    }

    CompositeNode* Manipulator::ConstructCoral(ResourceManager* resman_, std::string name_, glm::vec3 position_) {

        CompositeNode* coral = new CompositeNode(name_);
        coral->SetType(CompositeNode::Type::Coral);

        //creating main stems

        SceneNode* stem_prime = CreateSceneNodeInstance("Stem_Prime", "SuperLongStem", "ObjectMaterial", "", resman_);

        stem_prime->SetPosition(position_);
        stem_prime->SetOrientation(glm::normalize(glm::angleAxis(0.5f, glm::vec3(0.0, 0.0, 1.0))));
        stem_prime->SetColor(glm::vec3(0.97, 0.51, 0.47));
        //std::cout << "this is the main stems color: " << glm::to_string(stem_prime->GetColor()) << std::endl;
        //stem_prime->SetLength(10.0);
        stem_prime->SetPivot(glm::vec3(0, 5, 0));
        stem_prime->SetScale(glm::vec3(0.2, 0.2, 0.2));
        coral->SetRoot(stem_prime);

        SceneNode* stem1 = CreateSceneNodeInstance("Stem1", "LongStem", "ObjectMaterial", "", resman_);
        stem1->SetPosition(glm::vec3(2.5, -4.5, 0));
        stem1->Rotate(glm::normalize(glm::angleAxis(1.5f, glm::vec3(0, 0, 1))));
        stem1->SetColor(glm::vec3(0.97, 0.51, 0.47));
        stem_prime->AddChild(stem1);
        coral->AddNode(stem1);

        SceneNode* stem2 = CreateSceneNodeInstance("Stem1", "LongStem", "ObjectMaterial", "", resman_);
        stem2->SetPosition(glm::vec3(6.8, -6.0, 0));
        stem2->Rotate(glm::normalize(glm::angleAxis(1.0f, glm::vec3(0, 0, 1))));
        stem2->SetColor(glm::vec3(0.97, 0.51, 0.47));
        stem_prime->AddChild(stem2);
        coral->AddNode(stem2);

        SceneNode* stem3 = CreateSceneNodeInstance("Stem1", "LongStem", "ObjectMaterial", "", resman_);
        stem3->SetPosition(glm::vec3(1.7, -3.5, 0));
        stem3->Rotate(glm::normalize(glm::angleAxis(1.0f, glm::vec3(0, 0, 1))));
        stem3->SetColor(glm::vec3(0.97, 0.51, 0.47));
        stem2->AddChild(stem3);
        coral->AddNode(stem3);

        SceneNode* stem4 = CreateSceneNodeInstance("Stem1", "LongStem", "ObjectMaterial", "", resman_);
        stem4->SetPosition(glm::vec3(1.9, 2.5, 0));
        stem4->Rotate(glm::normalize(glm::angleAxis(1.8f, glm::vec3(0, 0, 1))));
        stem4->SetColor(glm::vec3(0.97, 0.51, 0.47));
        stem2->AddChild(stem4);
        coral->AddNode(stem4);

        SceneNode* stem5 = CreateSceneNodeInstance("Stem1", "LongStem", "ObjectMaterial", "", resman_);
        stem5->SetPosition(glm::vec3(1.8, 2.5, 1.5));
        stem5->Rotate(glm::normalize(glm::angleAxis(-1.8f, glm::vec3(0, 1, 1))));
        stem5->SetColor(glm::vec3(0.97, 0.51, 0.47));
        stem2->AddChild(stem5);
        coral->AddNode(stem5);

        SceneNode* stem6 = CreateSceneNodeInstance("Stem1", "LongStem", "ObjectMaterial", "", resman_);
        stem6->SetPosition(glm::vec3(1.8, 2.5, -1.5));
        stem6->Rotate(glm::normalize(glm::angleAxis(1.8f, glm::vec3(0, 1, 1))));
        stem6->SetColor(glm::vec3(0.97, 0.51, 0.47));
        stem2->AddChild(stem6);
        coral->AddNode(stem6);

        SceneNode* stem7 = CreateSceneNodeInstance("Stem1", "LongStem", "ObjectMaterial", "", resman_);
        stem7->SetPosition(glm::vec3(1.8, 2.5, -1.5));
        stem7->Rotate(glm::normalize(glm::angleAxis(1.8f, glm::vec3(0, 1, 1))));
        stem7->SetColor(glm::vec3(0.97, 0.51, 0.47));
        stem_prime->AddChild(stem7);
        coral->AddNode(stem7);

        SceneNode* stem8 = CreateSceneNodeInstance("Stem1", "LongStem", "ObjectMaterial", "", resman_);
        stem8->SetPosition(glm::vec3(-1.8, 2.5, -1.5));
        stem8->Rotate(glm::normalize(glm::angleAxis(1.8f, glm::vec3(1, 1, 0))));
        stem8->SetColor(glm::vec3(0.97, 0.51, 0.47));
        stem_prime->AddChild(stem8);
        coral->AddNode(stem8);

        SceneNode* stem9 = CreateSceneNodeInstance("Stem1", "Branch", "ObjectMaterial", "", resman_);
        stem9->SetPosition(glm::vec3(1.0, 5, 0));
        stem9->Rotate(glm::normalize(glm::angleAxis(1.8f, glm::vec3(0, 0, 1))));
        stem9->SetColor(glm::vec3(0.97, 0.51, 0.47));
        stem_prime->AddChild(stem9);
        coral->AddNode(stem9);

        SceneNode* stem10 = CreateSceneNodeInstance("Stem1", "Branch", "ObjectMaterial", "", resman_);
        stem10->SetPosition(glm::vec3(1.0, 6, 0));
        stem10->Rotate(glm::normalize(glm::angleAxis(2.5f, glm::vec3(0, 0, 1))));
        stem10->SetColor(glm::vec3(0.97, 0.51, 0.47));
        stem_prime->AddChild(stem10);
        coral->AddNode(stem10);

        SceneNode* stem11 = CreateSceneNodeInstance("Stem1", "Branch", "ObjectMaterial", "", resman_);
        stem11->SetPosition(glm::vec3(-0.6, 6, 0));
        stem11->Rotate(glm::normalize(glm::angleAxis(3.5f, glm::vec3(0, 0, 1))));
        stem11->SetColor(glm::vec3(0.97, 0.51, 0.47));
        stem_prime->AddChild(stem11);
        coral->AddNode(stem11);

        return coral;
    }


    CompositeNode* Manipulator::ConstructPart(ResourceManager* resman_, std::string name_, glm::vec3 position_) {

        CompositeNode* part = new CompositeNode(name_);
        part->SetType(CompositeNode::Type::Part);

        SceneNode* root_node = CreateSceneNodeInstance("root_node", "MainBody", "CombinedMaterial", "MetalTexture", resman_);

        root_node->SetPosition(position_);
        root_node->SetColor(glm::vec3(0.501, 0.501, 0.501));
        part->SetRoot(root_node);
        //part->AddNode(root_node);

        SceneNode* node1 = CreateSceneNodeInstance("root_node", "Exhaust", "CombinedMaterial", "MetalTexture", resman_);

        node1->SetPosition(glm::vec3(0.0, 6, 0));
        node1->SetColor(glm::vec3(0.501, 0.501, 0.501));
        root_node->AddChild(node1);
        part->AddNode(node1);

        SceneNode* node2 = CreateSceneNodeInstance("root_node", "Exhaust", "CombinedMaterial", "MetalTexture", resman_);

        node2->SetPosition(glm::vec3(-1.5, 5, 0));
        node2->SetColor(glm::vec3(0.501, 0.501, 0.501));
        root_node->AddChild(node2);
        part->AddNode(node2);

        SceneNode* node3 = CreateSceneNodeInstance("root_node", "Exhaust", "CombinedMaterial", "MetalTexture", resman_);

        node3->SetPosition(glm::vec3(1.0, 1.5, 0));
        node3->SetOrientation(glm::normalize(glm::angleAxis(-1.0f, glm::vec3(0, 0, 1))));
        node3->SetColor(glm::vec3(0.501, 0.501, 0.501));
        node1->AddChild(node3);
        part->AddNode(node3);

        return part;
    }

    CompositeNode* Manipulator::ConstructAnemonie(ResourceManager* resman_, std::string name_, glm::vec3 position_) {
        CompositeNode* an = new CompositeNode(name_);
        an->SetType(CompositeNode::Type::Anemonie);

        SceneNode* root_node = CreateSceneNodeInstance("root_node", "Base", "ObjectMaterial", "", resman_);

        root_node->SetPosition(position_);
        root_node->SetColor(glm::vec3(1, 0.83, 0.501));
        an->SetRoot(root_node);

        SceneNode* node1 = CreateSceneNodeInstance("root_node", "Middle", "ObjectMaterial", "", resman_);

        node1->SetPosition(glm::vec3(0, 0.6, 0));
        node1->SetColor(glm::vec3(1, 0.83, 0.501));
        root_node->AddChild(node1);
        an->AddNode(node1);

        float theta = 2 * glm::pi<float>();
        float offset = 0.2f;
        float stem_len = 2.0f;

        for (int i = 0; i < 10; i++) {
            float frac = float(i) / (10);
            SceneNode* branch = CreateSceneNodeInstance("Branch", "Cylinder", "ObjectMaterial", "", resman_);
            branch->SetType(SceneNode::Type::KelpStem); // type specified for shader
            branch->SetPosition(glm::vec3(0, 1, 0)); // 2 units above the root
            branch->SetPivot(glm::vec3(0, -stem_len / 2, 0));
            // Orbit about the pivot to set the starting orientation
            branch->Orbit(glm::angleAxis(theta / 12, glm::vec3(1.5 * cos(theta * frac), 0, 1.5 * sin(theta * frac))));
            branch->SetColor(glm::vec3(1, 0.654, 0.043));
            node1->AddChild(branch);
            an->AddNode(branch);

            for (int k = 0; k < 4; k++) {
                float frac = float(k) / 4;
                SceneNode* sub_branch = CreateSceneNodeInstance("SubBranch", "Tentacle", "ObjectMaterial", "", resman_);
                sub_branch->SetType(SceneNode::Type::KelpStem);
                sub_branch->SetPosition(glm::vec3(0, 1, 0));
                sub_branch->SetPivot(glm::vec3(0, -(0.5), 0));
                sub_branch->Orbit(glm::angleAxis(-theta / 16.0f, glm::vec3(cos(theta * frac), 0, sin(theta * frac))));
                sub_branch->SetColor(glm::vec3(1, 0.83, 0.501));
                branch->AddChild(sub_branch);
                an->AddNode(sub_branch);



            }

        }

        return an;
    }

        CompositeNode* Manipulator::ConstructSeaweed(ResourceManager * resman_, std::string name_, int length_compexity, glm::vec3 position_) {

            CompositeNode* seaweed = new CompositeNode(name_);
            seaweed->SetType(CompositeNode::Type::Seaweed);

            // Create root node
            SceneNode* root = CreateSceneNodeInstance("Root", "LowResCylinder", "KelpMaterial", "", resman_);
            root->SetScale(glm::vec3(1.0, 2.0, 1.0));
            root->SetPosition(position_);
            root->SetPivot(glm::vec3(0, -2.0, 0));
            seaweed->SetRoot(root);

            for (int i = 2; i < length_compexity + 1; i++) {
                SceneNode* piece = CreateSceneNodeInstance("Piece", "LowResCylinder", "KelpMaterial", "", resman_);
                piece->Scale(glm::vec3(1.0 / i + 0.25, i, 1.0 / i + 0.25));
                piece->Translate(glm::vec3(0, i - 1, 0));
                piece->SetPivot(glm::vec3(0, -(i - 1), 0));
                root->AddChild(piece);
                seaweed->AddNode(piece);
            }

            return seaweed;

        }

        CompositeNode* Manipulator::ConstructRock(ResourceManager* resman_, std::string name_, glm::vec3 position_) {

            CompositeNode* rock = new CompositeNode(name_);
            rock->SetType(CompositeNode::Type::Rock);

            // Create root node
            SceneNode* root = CreateSceneNodeInstance("Root", "Rock_Sphere", "NormalMapMaterial", "NormalMapRock", resman_);
           // root->SetScale(glm::vec3(1.0, 2.0, 1.0));
            root->SetPosition(position_);
            root->SetColor(glm::vec3(0.49, 0.498, 0.486));
            rock->SetRoot(root);

            return rock;

        }

        // (2) Animate hierarchical objects
        void Manipulator::AnimateAll(SceneGraph * scene_, double time_, float theta_) {
            CompositeNode* current_;
            for (int i = 0; i < scene_->GetSize(); i++) {
                current_ = scene_->GetNode(i);
                if (current_ != nullptr) {

                    // Animate all kelp instances
                    if (current_->GetType() == CompositeNode::Type::Kelp) {
                        AnimateKelp(current_, time_, theta_);
                    }

                    else if (current_->GetType() == CompositeNode::Type::Seaweed) {
                        AnimateKelp(current_, time_, theta_);
                    }

                }
            }
        }

        void Manipulator::AnimateKelp(CompositeNode * node_, double time_, float theta_) {
            node_->Orbit(glm::angleAxis(theta_, glm::vec3(0.1 * sin(time_), 0, 0.05 * sin(1 - time_))));
            for (int i = 0; i < node_->GetRoot()->GetChildCount(); i++) {
                node_->GetRoot()->GetChild(i)->Orbit(glm::angleAxis(theta_, glm::vec3(0.1 * sin(time_), 0, 0.05 * sin(1 - time_))));
            }
        }

        void Manipulator::AnimateSeaweed(CompositeNode * node_, double time_, float theta_) {
            node_->Orbit(glm::angleAxis(theta_, glm::vec3(1, 1, 1)));
            for (int i = 0; i < node_->GetRoot()->GetChildCount(); i++) {
                node_->GetRoot()->GetChild(i)->Orbit(glm::angleAxis(theta_, glm::vec3(1, 1, 1)));
            }
        }

        // Copied from game.cpp
        SceneNode* Manipulator::CreateSceneNodeInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name, ResourceManager * resman_) {

            // Get resources
            Resource* geom = resman_->GetResource(object_name);
            if (!geom) {
                throw(GameException(std::string("Could not find resource \"") + object_name + std::string("\"")));
            }

            Resource* mat = resman_->GetResource(material_name);
            if (!mat) {
                throw(GameException(std::string("Could not find resource \"") + material_name + std::string("\"")));
            }

            Resource* tex = NULL;
            if (texture_name != "") {
                tex = resman_->GetResource(texture_name);
                if (!tex) {
                    throw(GameException(std::string("Could not find resource \"") + material_name + std::string("\"")));
                }
            }

            // Create object instance
            SceneNode* obj = new SceneNode(entity_name, geom, mat, tex, 0);
            return obj;
        }
}