#pragma once
#include <cstdint>
#include <assert.h>

#define MAX_BITMAP_SIZE       24576
#define CHECK_BIT(value, bit) ((value >> bit) & 1)
#define BITMAP_NO_BITS_LEFT   0xFFFFFFFF

template <size_t SIZE> class Bitmap {
private:
	uint8_t m_bitmap_data[SIZE];
	size_t m_size = SIZE;

public:
	void init();
	void set_used(unsigned position);
	void set_unused(unsigned position);
	unsigned find_unused(unsigned search_start = 0);
	unsigned find_used(unsigned search_start = 0);
};


template <size_t SIZE> void Bitmap<SIZE>::init() {
	memset(m_bitmap_data, 0, sizeof(m_bitmap_data));
}


template <size_t SIZE> void Bitmap<SIZE>::set_used(unsigned position) {
	assert(position < m_size);
	m_bitmap_data[position / 8] |= (1 << (position % 8));
}

template <size_t SIZE> void Bitmap<SIZE>::set_unused(unsigned position) {
	assert(position < m_size);
	m_bitmap_data[position / 8] &= ~(1 << (position % 8));
}

template <size_t SIZE> unsigned Bitmap<SIZE>::find_unused(unsigned search_start) {
	assert(search_start < m_size);
	size_t bit_index = search_start;
	while (bit_index < m_size) {
		if (m_bitmap_data[bit_index / 8] == 0xFF)
			bit_index += 8;
		if (!CHECK_BIT(m_bitmap_data[bit_index / 8], bit_index % 8))
			return bit_index;

		bit_index++;
	}
	return BITMAP_NO_BITS_LEFT;
}

template <size_t SIZE> unsigned Bitmap<SIZE>::find_used(unsigned search_start) {
	assert(search_start < m_size);
	size_t bit_index = search_start;
	while (bit_index < m_size) {
		if (m_bitmap_data[bit_index / 8] == 0)
			bit_index += 8;
		if (CHECK_BIT(m_bitmap_data[bit_index / 8], bit_index % 8))
			return bit_index;

		bit_index++;
	}
	return BITMAP_NO_BITS_LEFT;
}
