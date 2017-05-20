#ifndef MY_RAND_H
#define MY_RAND_H

#include <climits>

// Xor-Shift
class my_rand {
private:
	unsigned int s_[4];

	inline unsigned int get_i() {
		const unsigned int t = s_[0] ^ (s_[0] << 11);
		s_[0] = s_[1];
		s_[1] = s_[2];
		s_[2] = s_[3];
		return s_[3] = (s_[3] ^ (s_[3] >> 19)) ^ (t ^ (t >> 8));
	}
public:
	my_rand(const unsigned int init = 0) {
		unsigned int s = init;
		s_[0] = s = 1812433253U * (s ^ (s >> 30)) + 1;
		s_[1] = s = 1812433253U * (s ^ (s >> 30)) + 2;
		s_[2] = s = 1812433253U * (s ^ (s >> 30)) + 3;
		s_[3] = s = 1812433253U * (s ^ (s >> 30)) + 4;
	}

	inline double get() { return (double)get_i() / UINT_MAX; }
};

#endif // !MY_RAND_H
