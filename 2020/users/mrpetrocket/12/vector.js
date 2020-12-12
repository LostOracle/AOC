// add two vectors, return the result
function addVector(vector1, vector2) {
    return createVector(
        vector1.x + vector2.x,
        vector1.y + vector2.y,
    );
}

function createVector(x, y) {
    return {x, y};
}

// rotate a vector around the origin (0, 0)
// degrees once again is always a multiple of 90
function rotateAroundOrigin(vector, degrees) {
    if (degrees < 0) degrees = 360 - Math.abs(degrees);
    if (degrees === 90) {
        // this seems too easy but if you look at it on a piece of paper it works
        return createVector(-vector.y, vector.x);
    }
    if (degrees === 180) return createVector(-vector.x, -vector.y);
    if (degrees === 270) {
        return rotateAroundOrigin(rotateAroundOrigin(vector, 180), 90);
    }
    throw new Error(`Unexpected value for degrees: ${degrees}`);
}

module.exports = {
    addVector,
    createVector,
    rotateAroundOrigin,
};
