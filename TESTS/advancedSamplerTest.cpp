// test to validate standalone samplers (Sampling from an External Image),
// & multiple image views used on a single sampler
/*
A VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE descriptor only holds an image view, 
and the sampler must be provided separately. This is useful if you want to use the same 
texture with different samplers (e.g., different filtering or addressing modes).
*/
int main() {
	// TODO
}