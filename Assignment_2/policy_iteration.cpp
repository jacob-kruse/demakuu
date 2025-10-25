// Copyright 2025 Jacob Kruse

#include <array>    // array variable
#include <cmath>    // NAN, abs()
#include <cstdlib>  // rand() and srand() functions
#include <ctime>    // time() function
#include <iostream> // string variable, cout(), endl()
#include <limits>   // negative infinity
#include <utility>  // pair variable
#include <vector>   // vector variable

using std::array;
using std::cout;
using std::endl;
using std::max;
using std::min;
using std::numeric_limits;
using std::pair;
using std::string;
using std::vector;

vector<pair<pair<int, int>, float>> get_transitions(pair<int, int> state,
                                                    string action) {
  // Define variables
  pair<int, int> out_of_bounds = {1, 2};
  vector<pair<pair<int, int>, float>> transitions;
  pair<pair<int, int>, float> transition_1, transition_2, transition_3,
      transition_combo;

  if (action == "Up") {
    // Add to the y value (move up) unless we are at the top of the grid, assign
    // probability of 0.85
    transition_1 = {{state.first, min(state.second + 1, 3)}, 0.85};

    // Subtract from the x value (move left) unless we are at the far left of
    // the grid, assign probability of 0.075
    transition_2 = {{max(state.first - 1, 0), state.second}, 0.075};

    // Add to the x value (move right) unless we are at the far right of the
    // grid, assign probability of 0.075
    transition_3 = {{min(state.first + 1, 3), state.second}, 0.075};
  } else if (action == "Down") {
    // Subtract from the y value (move down) unless we are at the bottom of the
    // grid, assign probability of 0.85
    transition_1 = {{state.first, max(state.second - 1, 0)}, 0.85};

    // Subtract from the x value (move left) unless we are at the far left of
    // the grid, assign probability of 0.075
    transition_2 = {{max(state.first - 1, 0), state.second}, 0.075};

    // Add to the x value (move right) unless we are at the far right of the
    // grid, assign probability of 0.075
    transition_3 = {{min(state.first + 1, 3), state.second}, 0.075};
  } else if (action == "Left") {
    // Subtract from the x value (move left) unless we are at the far left of
    // the grid, assign probability of 0.85
    transition_1 = {{max(state.first - 1, 0), state.second}, 0.85};

    // Add to the y value (move up) unless we are at the top of the grid, assign
    // probability of 0.075
    transition_2 = {{state.first, min(state.second + 1, 3)}, 0.075};

    // Subtract from the y value (move down) unless we are at the bottom of the
    // grid, assign probability of 0.075
    transition_3 = {{state.first, max(state.second - 1, 0)}, 0.075};
  } else if (action == "Right") {
    // Add to the x value (move right) unless we are at the far right of the
    // grid, assign probability of 0.85
    transition_1 = {{min(state.first + 1, 3), state.second}, 0.85};

    // Add to the y value (move up) unless we are at the top of the grid, assign
    // probability of 0.075
    transition_2 = {{state.first, min(state.second + 1, 3)}, 0.075};

    // Subtract from the y value (move down) unless we are at the bottom of the
    // grid, assign probability of 0.075
    transition_3 = {{state.first, max(state.second - 1, 0)}, 0.075};
  }

  if (transition_1.first == out_of_bounds) {
    // Set the transition to the original state because we cannot travel out of
    // bounds
    transition_1.first = state;
  } else if (transition_2.first == out_of_bounds) {
    // Set the transition to the original state because we cannot travel out of
    // bounds
    transition_2.first = state;
  } else if (transition_3.first == out_of_bounds) {
    // Set the transition to the original state because we cannot travel out of
    // bounds
    transition_3.first = state;
  }

  if (transition_1.first == transition_2.first) {
    // Combine the probabilities of the identical resulting states
    transition_combo = {state, 0.925};

    // Define the list of transitions with the combined and remaining transition
    transitions = {transition_combo, transition_3};
  } else if (transition_1.first == transition_3.first) {
    // Combine the probabilities of the identical resulting states
    transition_combo = {state, 0.925};

    // Define the list of transitions with the combined and remaining transition
    transitions = {transition_combo, transition_2};
  } else if (transition_2.first == transition_3.first) {
    // Combine the probabilities of the identical resulting states
    transition_combo = {state, 0.15};

    // Define the list of transitions with the combined and remaining transition
    transitions = {transition_1, transition_combo};
  } else {
    // Define the list of transitions as the three defined transitions
    transitions = {transition_1, transition_2, transition_3};
  }

  return transitions;
}

float get_reward(pair<int, int> state, string action,
                 pair<int, int> transition) {
  // Define variables
  float reward = 0.0;
  pair<int, int> pit = {0, 3};
  pair<pair<int, int>, pair<int, int>> goals = {{3, 0}, {3, 3}};

  if (transition == goals.first || transition == goals.second) {
    // Assign the reward for reaching a goal state
    reward = 1;
  } else if (transition == pit) {
    // Assign the reward for reaching the pit/bad state
    reward = -1;
  } else {
    // Assign the reward for reaching any other state
    reward = -0.05;
  }

  return reward;
}

float get_expected_reward(pair<int, int> state, string action) {
  // Define variable to hold sum of expected rewards
  float expected_reward = 0.0;

  // Get the transition probabilities for the action at the state
  vector<pair<pair<int, int>, float>> transitions =
      get_transitions(state, action);

  // Loop through each transition to get the expected reward
  for (pair<pair<int, int>, float> transition : transitions) {
    // Multiply the probability of the transition by the reward of reaching the
    // transition and add it to the sum
    expected_reward +=
        transition.second * get_reward(state, action, transition.first);
  }

  return expected_reward;
}

array<array<float, 4>, 4>
policy_evaluator(array<array<string, 4>, 4> policy,
                 array<array<float, 4>, 4> prev_value_function,
                 vector<pair<int, int>> states, array<string, 4> actions) {
  // Define loop timing variables
  bool converged = false;
  int iteration = 0;

  // Define variable for value function
  array<array<float, 4>, 4> value_function = {};

  // Loop as long as it has not converged or after 10,000 iterations
  while (!converged && iteration < 10000) {
    // Loop  through all states
    for (pair<int, int> state : states) {
      // Get the policy's action for the current state
      string action = policy[state.first][state.second];

      // Get the possible transitions for the policy's action at the current
      // state
      vector<pair<pair<int, int>, float>> transitions =
          get_transitions(state, action);

      // Get the immediate expected reward for the action output from the policy
      // at the current state
      float immediate_expected_reward = get_expected_reward(state, action);

      // Define a variable for upcoming loop
      float expected_value = 0.0;

      // Loop through each possible transition
      for (pair<pair<int, int>, float> transition : transitions) {
        // Multiiply the discount factor by the probability of the transition
        // and by the value of the transitions state
        float transition_value = 0.99 * transition.second *
                                 prev_value_function[transition.first.first]
                                                    [transition.first.second];

        // Add the transition value to the sum
        expected_value = expected_value + transition_value;
      }

      // Assign the new value to the value function
      value_function[state.first][state.second] =
          immediate_expected_reward + expected_value;
    }

    // Define variable for upcoming loop
    float max_difference = 0.0;

    // Loop through each state now that we have defined the previous and current
    // value functions
    for (pair<int, int> state : states) {
      // Calculate the difference of the value functions at the state
      float difference = abs(value_function[state.first][state.second] -
                             prev_value_function[state.first][state.second]);

      // If the current difference is greater than the maximum difference,
      // modify the maximum difference
      if (difference > max_difference) {
        max_difference = difference;
      }
    }

    // Check for convergence
    if (max_difference < 0.000001) {
      converged = true;
    }

    // After completing loop, iterate the iterations variable and assign the
    // value function to prev_value_function for next loop
    iteration++;
    prev_value_function = value_function;
  }

  return value_function;
}

array<array<string, 4>, 4>
policy_iterator(array<array<string, 4>, 4> policy,
                array<array<float, 4>, 4> prev_value_function,
                vector<pair<int, int>> states, array<string, 4> actions) {
  // Define loop timing variable
  bool unchanged = false;

  // Loop so long as the policy has been modified
  while (!unchanged) {
    // Get the value function for the policy at each state
    array<array<float, 4>, 4> value_function =
        policy_evaluator(policy, prev_value_function, states, actions);

    // Define counter variable to track number of changes to policy
    int changes = 0;

    // Loop through each state
    for (pair<int, int> state : states) {
      // Get the policy's action for the current state
      string action = policy[state.first][state.second];

      // Get the transition probabilities for the policy action at the state
      vector<pair<pair<int, int>, float>> policy_transitions =
          get_transitions(state, action);

      // Get the immediate expected reward for the action output from the policy
      // at the current state
      float policy_value = get_expected_reward(state, action);

      // Loop through each transition
      for (pair<pair<int, int>, float> transition : policy_transitions) {
        // Get the expected value for the current transition
        float transition_value =
            value_function[transition.first.first][transition.first.second] *
            transition.second;

        // Add the current transition's expected value to the running total
        // expected value
        policy_value = policy_value + transition_value;
      }

      // Define the max_action variable and set the max action value to negative
      // infinity
      string max_action;
      float max_action_value = -numeric_limits<float>::infinity();

      // Loop through each action
      for (string action : actions) {
        // Get the transition probabilities for the current action at the state
        vector<pair<pair<int, int>, float>> action_transitions =
            get_transitions(state, action);

        // Get the immediate expected reward for the action output from the
        // policy at the current state
        float action_value = get_expected_reward(state, action);

        // Loop through each transition
        for (pair<pair<int, int>, float> transition : action_transitions) {
          // Get the expected value for the current transition
          float transition_value =
              value_function[transition.first.first][transition.first.second] *
              transition.second;

          // Add the current transition's expected value to the running total
          // expected value
          action_value = action_value + transition_value;
        }

        // If the current action value is greater than the current maximum
        // action value
        if (action_value > max_action_value) {
          // Assign the action and corresponding value to the maximum variables
          max_action = action;
          max_action_value = action_value;
        }
      }

      // If there is a more valuable action than the one output from the policy
      if (max_action_value > policy_value) {
        // Modify the policy to do this action at the current state
        policy[state.first][state.second] = max_action;

        // Increment the changes variable to track the number of changes to the
        // policy
        changes++;
      }
    }

    // If there weren't any changes to the policy
    if (changes == 0) {
      // Set the unchanged variable to true to exit the loop
      unchanged = true;
    }

    // Assign the current value function to the previous function variable to be
    // used in next loop
    prev_value_function = value_function;
  }

  return policy;
}

array<array<string, 4>, 4> initialize_policy(vector<pair<int, int>> states,
                                             array<string, 4> actions) {
  // Define a variable for the policy
  array<array<string, 4>, 4> policy = {};

  // Seed the random number generator
  srand(static_cast<unsigned int>(time(nullptr)));

  // Loop  through all states
  for (pair<int, int> state : states) {
    // Generate a random index between 0 and 3
    int random_index = std::rand() % 4;

    // Get a random action
    string action = actions[random_index];

    // Assign the action for the current state
    policy[state.first][state.second] = action;
  }

  return policy;
}

int main() {
  // Define all possible states
  vector<pair<int, int>> states = {
      {0, 0}, {0, 1}, {0, 2}, {0, 3}, {1, 0}, {1, 1}, {1, 3}, {2, 0},
      {2, 1}, {2, 2}, {2, 3}, {3, 0}, {3, 1}, {3, 2}, {3, 3},
  };

  // Define all possible actions
  array<string, 4> actions = {"Up", "Down", "Left", "Right"};

  // Initialize the policy randomly
  array<array<string, 4>, 4> initial_policy =
      initialize_policy(states, actions);

  // Initialize the value function to be 0 at all states, NAN at out of bounds
  array<array<float, 4>, 4> value_function = {{{0.0, 0.0, 0.0, 0.0},
                                               {0.0, 0.0, NAN, 0.0},
                                               {0.0, 0.0, 0.0, 0.0},
                                               {0.0, 0.0, 0.0, 0.0}}};

  // Pass the policy, value function, states, and actions to the policy iterator
  array<array<string, 4>, 4> converged_policy =
      policy_iterator(initial_policy, value_function, states, actions);

  // Print statements
  cout << "Converged Policy\n---------------------------" << endl;

  // Print the resulting policy
  for (pair<int, int> state : states) {
    cout << "State: (" << state.first << "," << state.second
         << ")  Action: " << converged_policy[state.first][state.second]
         << endl;
  }

  return 1;
}
