#include "RRR.h"

RRR::RRR(string &bits) {
	// number of bits in input string
	uint64_t n = bits.size();
	if (n > 3) {
		// bit number in each block
		this->blockSize = (uint32_t)(floor(log2(bits.size())) / 2);
	}
	else {
		this->blockSize = 1;
	}
	this->bitsForClass = (uint32_t)ceil(log2(this->blockSize + 1));
	this->bitsForClass = this->bitsForClass == 0 ? 1 : this->bitsForClass;

	// bit number in super blocks
	this->superBlockSize = (uint32_t)(pow(floor(log2(n)), 2.));
	this->blocksPerSuperBlock = this->superBlockSize / this->blockSize;

	// initialize table
	RRRTable::getInstance();
	if (RRRTable::initialBlockLength == 0) {
		RRRTable::getInstance().createTable(this->blockSize);
	}

	uint64_t blockContent = 0;
	uint64_t currentBitInContent = ((uint64_t)1) << 63;
	uint64_t currentContentElement = 0;
	//uint64_t currentIndex = 0;
	uint64_t alignValue = 0;

	uint64_t blockRank = 0;
	uint64_t superBlockRank = 0;

	superBlock superBlock(0, 0);
	uint64_t blockIndex = 0;
	uint64_t superBlockOffset = 0;

	for (uint64_t i = 0; i < n; i++) {
		blockContent = (blockContent << 1);

		if (bits[(unsigned int)i] == '1') {
			blockContent = (blockContent | 1);
			blockRank++;
		}

		if ((i != 0 && ((i+1) % this->blockSize) == 0) || (i+1 == n)) {
			blockIndex++;
			uint32_t currentBlockSize = this->blockSize;
			if (i + 1 == n && ((i+1) % this->blockSize)) {
				blockContent = blockContent << (this->blockSize - ((i + 1) % this->blockSize));
			}
			// blockSize used for bits alignment for table search
			uint32_t blockOffset = RRRTable::getInstance().getOffset((uint32_t)blockRank, (uint32_t)blockContent, this->blockSize);
			uint32_t bitsForOffset = RRRTable::getInstance().getBitsForOffset(blockRank);

			superBlockOffset += (bitsForOffset + this->bitsForClass);

			uint64_t blockRankCopy = blockRank << (64 - this->bitsForClass - alignValue);
			for (uint32_t k = 0; k < this->bitsForClass; k++) {
				currentContentElement = currentContentElement | (currentBitInContent & blockRankCopy);
				currentBitInContent = currentBitInContent >> 1;
				alignValue++;
				//currentIndex++;
				if (currentBitInContent == 0) {
					// i need to add another uint64_t into the content vector
					content.push_back(currentContentElement);
					currentContentElement = 0;
					currentBitInContent = ((uint64_t)1) << 63;
					alignValue = 0;
					blockRankCopy = blockRank << (64 - this->bitsForClass + k + 1);
					currentContentElement = currentContentElement | (currentBitInContent & blockRankCopy);
					currentBitInContent = currentBitInContent >> 1;
					alignValue++;
					k++;
				}
			}

			if (i + 1 == n && ((i + 1) % this->blockSize)) {
				int a = 33;
			}

			uint64_t blockOffsetCopy = ((uint64_t)blockOffset) << (64 - bitsForOffset - alignValue);
			for (uint32_t k = 0; k < bitsForOffset; k++) {
				currentContentElement = currentContentElement | (currentBitInContent & blockOffsetCopy);
				currentBitInContent = currentBitInContent >> 1;
				alignValue++;
				//currentIndex++;
				if (currentBitInContent == 0 || (i + 1) == n) {
					// i need to add another uint64_t into the content vector
					content.push_back(currentContentElement);
					currentContentElement = 0;
					currentBitInContent = ((uint64_t)1) << 63;
					alignValue = 0;
					blockOffsetCopy = blockOffset << (64 - bitsForOffset + k + 1);
					currentContentElement = currentContentElement | (currentBitInContent & blockOffsetCopy);
					currentBitInContent = currentBitInContent >> 1;
					alignValue++;
					k++;
				}
			}
			
			superBlockRank += blockRank;
			if ((blockIndex % this->blocksPerSuperBlock == 0) || (i + 1 == n)) {
				superBlocks.push_back(superBlock);

				superBlock.second = superBlockOffset;
				superBlock.first = superBlockRank;
			}

			blockRank = 0;
			blockContent = 0;
		}
	}
}

uint64_t RRR::rank1(uint64_t index) {
	// calculate block index
	uint64_t ib = index / this->blockSize;

	// calculate which superblock the ib-th block resides in
	uint32_t is = (uint32_t) (ib / this->blocksPerSuperBlock);

	// set rankSum to the rank contained in the current superblock
	uint64_t rankSum = this->superBlocks[is].first;

	// used to know in which block we current are in order to add the current block rank to the rankSum
	uint64_t currentBlockIndex = this->blocksPerSuperBlock * is;
	uint64_t currentIndex = this->superBlocks[is].second;
	uint64_t contentIndex = currentIndex / 64;
	uint64_t currentIndexInContentElement = currentIndex % 64;
	uint64_t currentContentElement = this->content[(uint32_t)contentIndex];
	uint64_t mask = (((uint64_t)1) << this->bitsForClass) - 1;

	//currentIndexInContentElement - this->bitsForClass < 0

	// sum up block ranks up to the ib-th block
	while (currentBlockIndex <= ib) {
		uint64_t tempRank = currentContentElement >> (64 - this->bitsForClass - currentIndexInContentElement);
		uint64_t leftOverBits = currentIndexInContentElement + this->bitsForClass;
		if (leftOverBits >= 64) {
			// we have overflow
			contentIndex++;
			currentContentElement = this->content[(uint32_t)contentIndex];
			currentIndexInContentElement = leftOverBits % 64;
			tempRank = tempRank | (currentContentElement >> (64 - currentIndexInContentElement));
		}
		else {
			currentIndexInContentElement += this->bitsForClass;
		}
		tempRank = tempRank & mask;
		if (currentBlockIndex != ib) {
			rankSum += tempRank;
		}



		uint32_t bitsForOffset = RRRTable::getInstance().getBitsForOffset(tempRank);
		if (currentBlockIndex != ib) {
			currentIndexInContentElement += bitsForOffset;
			if (currentIndexInContentElement >= 64) {
				// we have overflow
				contentIndex++;
				currentContentElement = this->content[(uint32_t)contentIndex];
				currentIndexInContentElement = currentIndexInContentElement % 64;
			}
		}
		else {
			uint64_t maskForOffset = (((uint64_t)1) << bitsForOffset) - 1;
			uint64_t tempOffset = currentContentElement >> (64 - bitsForOffset - currentIndexInContentElement);
			leftOverBits = currentIndexInContentElement + bitsForOffset;
			if (leftOverBits >= 64) {
				// we have overflow
				contentIndex++;
				currentContentElement = this->content[(uint32_t)contentIndex];
				currentIndexInContentElement = leftOverBits % 64;
				tempOffset = tempOffset | (currentContentElement >> (64 - currentIndexInContentElement));
			}

			tempOffset = tempOffset & maskForOffset;
			// We are in the ib-th block; read the appropriate rank from RRRTable
			uint64_t j = index % ((uint64_t)this->blockSize);
			rankSum += RRRTable::getInstance().getRankForBlockAtPosition(tempRank, tempOffset, (uint32_t)j);
			// get out of while loop
		}
		currentBlockIndex++;
	}
	return rankSum;
}

uint64_t RRR::rank0(uint64_t index) {
	return index - this->rank1(index) + 1;
}