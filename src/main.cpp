
#include <iostream>


//This system uses &value reference variable calls and returning the enum errorlevel so that the errorlevel is always the return and side effects enter through the arguments list only. 

namespace node_pool 
{
	
	enum class node_pool_error
	{
		None, //For diagnostic requirement (insert-requirement)
		Node_Zero_False, // Indicates a violation of the read-only contract for node 0, triggered by node 0 not being set to 0
		Node_Array_Value_Initialized_Error, // Indicates that one or more of the node array elements were not properly initialized.
		Node_Zero_Access_Attempt, // Indicates a violation of the read-only contract for node 0
		Node_Out_Of_Range_On_Set, // Indicates there was an attempt to access a node that is out of bounds
		Node_Set_Value_Mismatch, // Indicates a node's value was not corrently set during set_Node()
		DummyNonZeroGetNodeOutOfBounds //Indicates that in the test for out of bounds the dummy node was not set to 0
	};
	
	// Safety net — only works if compiler warns on unhandled enum
	static_assert(static_cast<int>(node_pool::node_pool_error::DummyNonZeroGetNodeOutOfBounds) == 6, "to_string out of sync");
	
	
	
	// REQ 1.1
	// REQ 
	//Node 0 must never be used. 
	//Node 0 will be initialized to known value of 0
	//If node 0 is ever altered an error flag is raised
	// REQ 
	constexpr std::size_t node_count = 1000;
	static_assert(node_count > 0, "node_count must be > 0");
	

	namespace node_pool_internal
	{
		static uint64_t Nodes[node_count] = {};
	}
	using node_pool_internal::Nodes;
	
	[[nodiscard]] node_pool_error are_Nodes_Initialized()
	{
		// Fully bounded loop, 0 to node_count - 1; termination guaranteed
		for (std::size_t i=0;i<node_count;i++)
		{
			if (Nodes[i] != 0U)
			{ 
				return node_pool_error::Node_Array_Value_Initialized_Error;
			}
		}
		return node_pool_error::None;	
	}
	
	[[nodiscard]] node_pool_error is_Node_Zero_Intact()
	{
		if (Nodes[0] != 0U){ return node_pool_error::Node_Zero_False; }
		return (node_pool_error::None);
	}
	
	// Function is side-effect free except for deterministic write to fixed-size array. No dynamic allocation, recursion, or exception used.
	[[nodiscard]] node_pool_error set_Node(std::size_t p_Node_ID, std::uint64_t p_Value)
	{
        if (p_Node_ID == 0)
		{
			return node_pool::Node_Zero_Access_Attempt;
		}
		if (p_Node_ID >= node_count)
        {
			return node_pool_error::Node_Out_Of_Range_On_Set;
        }
        Nodes[p_Node_ID] = p_Value;
        if (Nodes[p_Node_ID] != p_Value)
		{
			return node_pool_error::Node_Set_Value_Mismatch;
		}
		return node_pool_error::None;
	}
	
	//Getter for testing
	//Assume the return value of 0 to mean the node was not found, either the default value is still 0, or the node ID requested doesn't exist yet.
	[[nodiscard]] node_pool_error get_Node(std::size_t id, std::uint64_t& out_value)
	{
		if (id >= node_count)
		{
			out_value = 0U; // explicitly zeroed — optional but clear
			return node_pool_error::Node_Out_Of_Range_On_Set;
		}

		out_value = Nodes[id];
		return node_pool_error::None;
	}

const char* to_string(node_pool::node_pool_error e)
{
	using E = node_pool::node_pool_error;
	switch (e)
	{
		case E::None: return "None";
		case E::Node_Zero_False: return "Node_Zero_False";
		case E::Node_Array_Value_Initialized_Error: return "Node_Array_Value_Initialized_Error";
		case E::Node_Zero_Access_Attempt: return "Node_Zero_Access_Attempt";
		case E::Node_Out_Of_Range_On_Set: return "Node_Out_Of_Range_On_Set";
		case E::Node_Set_Value_Mismatch: return "Node_Set_Value_Mismatch";
		default: return "Unknown_Error";
	}
}

// ------------------------
// Tests
// ------------------------

// TEST: NODEPOOL-001
// REQ-ID: REQ-001, Description: Node 0 shall never be written to
node_pool::node_pool_error test_Node_Zero_Protection()
{
    if (node_pool::is_Node_Zero_Intact() != node_pool::node_pool_error::None)
	{
		return node_pool::node_pool_error::Node_Zero_False;
	}

    node_pool::node_pool_error result = node_pool::set_Node(0, 999); // Should fail
	if (result != node_pool::node_pool_error::Node_Zero_Access_Attempt)
	{
		return result; 
	}
	
    return node_pool::is_Node_Zero_Intact(); // Should still be true
}

// TEST: NODEPOOL-002
// REQ-ID: REQ-002, Description: Out of bounds request to get_Node should always return 0 upon failure
// REQ-ID: REQ-003, Description: Out of bounds request to get_Node should always set the Node_Out_Of_Range_On_Set state
 
node_pool::node_pool_error test_Get_Node_Out_Of_Bounds()
{
	std::uint64_t dummy = 0xDEADBEEF;
	auto err = node_pool::get_Node((node_pool::node_count + 999), dummy);
	if (err != node_pool::node_pool_error::Node_Out_Of_Range_On_Set)
		return err;
	if (dummy != 0U)
		return node_pool::node_pool_error::DummyNonZeroGetNodeOutOfBounds; // reused for "unexpected value"
	return node_pool::node_pool_error::None;
}

// ------------------------
// Test Runner Macro
// ------------------------

#define RUN_TEST(id, fn) \
    do { \
        auto err = fn(); \
        std::ofstream log("test_results.log", std::ios::app); \
        if (err != node_pool::node_pool_error::None) { \
            std::cout << "[FAIL] " << id << ": " << to_string(err) << "\n"; \
            log << id << ",FAIL," << to_string(err) << "\n"; \
            log.close(); \
            return 1; \
        } else { \
            std::cout << "[PASS] " << id << "\n"; \
            log << id << ",PASS,None\n"; \
            log.close(); \
        } \
    } while(0)

int main()
{
    RUN_TEST("NODEPOOL-INIT", node_pool::are_Nodes_Initialized);
    RUN_TEST("NODEPOOL-001", test_Node_Zero_Protection);
    RUN_TEST("NODEPOOL-002", test_Get_Node_Out_Of_Bounds);

    return 0;
}
