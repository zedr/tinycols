#ifndef TINYCOLS_RESULT_H
#define TINYCOLS_RESULT_H

/**
 * result - The result of an action affecting a piece.
 */
enum result {
	UNKNOWN,
	PENDING,
	ACTIVE,
	MOVED,
	BLOCKED,
	LANDED,
	PERSISTED,
	INVISIBLE
};

#endif //TINYCOLS_RESULT_H
