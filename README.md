# Floats In Depth

When looking at how C++ represents integers under-the-hood, it's rather self-explanitory on how it all works, since it's just the number in base-2. Floats, on the other hand, are a bit trickier to understand right away, and can cause some confusing and dangerous bugs if used incorrectly. 

An interesting anecdote of a dangerous floating-point error is from the US Patriot Missile software. Due to a floating-point issue, the longer the software was running, the more incorrect the results would be when looking for incoming missiles. This resulted in the death of 28 American soldiers. It's worth reading before continuing, so you'll begin to understand what happened with the erroneous code (viewable [here](http://fas.org/spp/starwars/gao/im92026.htm)).

In this section we will go over what makes a float tick, why the implementation was chosen, and the downsides of floats. 

## What are they?

In C++, floats use the IEEE Standard 754-2008, which is the most common way that floats are implemented on computers today. Behind the scenes, floats represent fractional numbers through scientific notation. That is to say, the float consists of a sign, exponent, and a mantissa. 

```
± 1.001 * 2 ^ 100
1 \_2_/   \__3__/

1. Sign
2. Mantissa
3. Exponent
```

This implementation allows for a huge range in numbers, and can change precision appropriately depending on the size of the number. 

## Memory layout

Floating point numbers come in two sizes: The single precision float (which is 32 bits), and the double precision float (which is 64 bits). Let's look at how these look in memory, and what each section of memory does:

   | Sign | Exponent | Mantissa | Total
--- | --- | --- | --- | ---
Single Precision | 1 bit | 8 bits | 23 bits | 32 bits total
Double Precision | 1 bit | 11 bits | 52 bits | 64 bits total

### Sign

The 'Sign' component takes up the first bit of the value, and determines whether it's a positive, or negative number. If the bit is 0, then the value is positive. If the bit is set, the value will be negative.

### Exponent

The exponent is the next component (sweet rhyme), and takes up 8 bits on a single precision value (11 bits on the double precision). The exponent of a floating-point value has a base of 2. 

```
1.001 * 2 ^ 100
        |
        base of 2
```

The exponent needs to be able to store both positive and negative numbers in a small amount of space. This is accomplished by adding a 'bias' to the exponent. 

Type | Bias
--- | ---
Single Precision | 127
Double Precision | 1023

For single precision, the value of 127 is interpreted as 0. Any offset below that becomes negative, and any offset over is seen as positive. For example:

```
Exponent: 127
100 * 2 ^ 0

Exponent: 138
100 * 2 ^ 11

Exponent: 120
100 * 2 ^ -7
```

In the later section "Special Values", we will look into a few reserved values of the exponent that denote a special floating-point value.

### Mantissa

The mantissa (sometimes referred to as the significand), represents the fractional component of the floating-point value. 

```
5.00 * 2 ^ 10
\__/
Mantissa
```

It is typically stored in what is called 'normalized form', meaning the decimal place is put immediately after the first non-zero number. However, since the mantissa is in binary, the first non-zero number will always be a 1, so we can implicitly have it there without it taking any space. This technically gives us 24 bits of precision while only using 23 bits!

## Working With Binary Fractions in the Mantissa

To understand the memory components of floats a bit better, it's good practice to convert back and forth from binary to decimal. Once you get the hang of this, floating-point values become much easier to reason about. If you ever need a companion program to help you reason about the conversions, I'd reccommend [this one](http://www.h-schmidt.net/FloatConverter/IEEE754.html), since it shows all the relevent data that goes in the conversion. 

### Converting a Mantissa Into a Decimal Value

There are a few steps in converting a floating-point value back to decimal. Let's break it up piece-by-piece and then bring it all together at the end. Keep in mind that this assumes a normalized floating-point value.

**Sign**

The sign needs to be able to make the result of our equation positive or negative depending on whether or not the bit is set. We can accomplish that like this:

```
((-1) ^ Sign) * ...
```

If we raise the number -1 to the power of 0, the result will be 1, and if we raise to the power of 1, the result will be -1. If we multiply the rest of the equation by this, we will get the intended effect.

**Exponent**

The exponent is simple. All we need to do is take into account the bias of the exponent (127 for single-precision) and the length of the mantissa (23 bits for single precision).

```
... * (2 ^ (Exponent - 127 - 23))
```

**Mantissa**

The mantissa has two different equations for it's two possible states: normalized and denormalized. Since normalized floats have an implicit leading 1, we need to take that into account in the equation. Just take the decimal value of the mantissa, and add the leading implicit 1 as (2 ^ 23):

```
... * ((2 ^ 23) + Mantissa) * ...
        |
        Implicit leading 1
```

It's the same idea with denormalized floats, but you can take away the value for the implicit leading 1:


```
... * (Mantissa) * ...
```

**Bringing it all together**

```
Normalized floating-point value to decimal
((-1) ^ Sign) * ((2 ^ 23) + Mantissa) * (2 ^ (Exponent - 127 - 23)) = decimal value

Denormalized floating-point value to decimal
((-1) ^ Sign) * (Mantissa) * (2 ^ (Exponent - 127 - 23)) = decimal value
```

**Example**

So now that we know how to convert a floating-point value into decimal, let's try out a small example to see how it works. 

```
0 10000001 01100000000000000000000

Sign: 0
Exponent: 10000001, 129 in decimal
Mantissa 01100000000000000000000, 3145728 in decimal

((-1) ^ 0) * ((2 ^ 23) + 3145728) * (2 ^ (129 - 127 - 23))
(1) * (11534336) * (2 ^ -21)
5.5

Therefore 0 10000001 01100000000000000000000 equals 5.5
```

### Converting a Decimal Value Into a Mantissa

Converting a decimal fraction into a binary mantissa is easy, though a bit tedious. To start, completely truncate the numbers to the left of the radix point so the value begins with a '0.', since we don't care about the non-fractional data. 

What you'll do after is continually multiply the value by 2, and each time the fractional data overflows into the left of the radix point, that will result in a 1 for the binary fraction.

This is easily explained with an example, so let's take a look at converting two numbers into binary. Feel free to try out the [Float Inspector](./FloatInspector.cpp) to verify these values and try out a few of your own!

**Example 1**

Let's look at how to convert the decimal number 1.65625 into decimal. First off, when converting the fraction, we can ignore the data to the left of the radix point. Now let's apply the method that I mentioned above:

Equation | Result | Binary
--- | --- | ---
0.65625 * 2 | 1.3125 | 1
0.3125 * 2 | 0.625 | 0
0.625 * 2 | 1.25 | 1
0.25 * 2 | 0.5 | 0
0.5 * 2 | 1.0 | 1

Once we get to the number 1.0, the fraction is done, since 0.0 * 2 is 0.0, which will go on forever. Now that we have the binary fraction, we can put the data to the left of the radix point back in. Since the data in this example was a 1, and there is already an implicit 1 in the mantissa, we don't have to change anything.

```
Sign: 0
Exponent: 127 - 0 = 127
Mantissa: 101010000000000000000000000000
```

**Example 2**

The previous example was a pretty easy one that worked itself out after only a few iterations. I'd like to show another example of a conversion that isn't quite as easy, and will also show one of the limitations of floating-point values. The number we'll convert next is 0.1:

Equation | Result | Binary
--- | --- | ---
0.1 * 2 | 0.2 | 0
0.2 * 2 | 0.4 | 0
0.4 * 2 | 0.8 | 0
0.8 * 2 | 1.6 | 1
0.6 * 2 | 1.2 | 1
0.2 * 2 | 0.4 | 0
... | ... | ...

Looks like we hit a recursive equation! The '001' will keep repeating over and over. Just like the fraction 1/3 can't be properly represented in base 10, 0.1 can't be represented properly in base 2! Since this will be cut off after 23 bits, there will be a small precision issue if you introduce this number into any of your equations. Keep in mind that not every number can be perfectly represented in a float!

Since normalized floats always start with an implicit "1.", we will have to move our result over -4 places so our first '1' will be the implicit bit, which will be our exponent! Also remember that floats will round the mantissa to represent the number as closely as possible if it doesn't completely fit within the 23 bits.

```
Mantissa Before: 00011001100110011001100...
Mantissa After:  10011001100110011001101

Exponent: -4 + 127(Bias) = 123 = 01111011

Final Value: 0 01111011 10011001100110011001101
```

## Floating-Point Precision

When you use a float, you're getting a much larger range AND fractional data while using the same amount of space as an integer. All this extra information comes at the cost of precision. 

An integer has 32 bits of precision, and stores data without any margin of error. Floats on the other hand, stores it's precision in the mantissa, giving it 24 bits of precision (23 without the inferred bit in normalized floats). Floats are only able to account for this loss of precision by truncating data past it's precision, and rounding up what it cannot store. 

For example:

```
32 bit integer:   10101010 00001111 11001100 10110100 (2,853,162,164 in decimal)
23 bit mantissa: 1.0101010 00001111 11001101
                 |                         |
                 Implicit leading 1        Rounded truncated data

Converted back: 10101010 00001111 11001101 00000000 (2,853,162,240 in decimal)
Difference: 76 
```

The larger the number you are trying to represent as a floating-point value, the less precision you will have after the radix point. This is because you are using up part of mantissa for digits to the left of the radix point, leaving fewer bits for the fractional data. Because the mantissa is in base-2, for every power of 2 that is hit to the left of the radix point, the right of the radix point will lose 1 bit of precision.

The numbers between 0 and 1 can be computed like this:

```
0..1 = Bias * (2 ^ MantissaLength)
```

And every distribution between two number afterwards (ex. 1 to 2) can be computed as such:

```
n..n+1 = 2 ^ (MantissaLength - P)
Where P = the exponent of the power of 2
```

Therefore the distribution from 8 to 9 can be computed as:

```
8 = 2 ^ 3
        |
        Exponent

8..9 = 2 ^ (23 - 3) = 1,048,576
                 |
                 Exponent
```

Following this formula for floating-point precision, let's look at which numbers have a changed precision, and compare the precision of single-precision and double-precision:

Range | Single Precision | Double Precision
 --- | --- | --- 
0 to 1 | 1,065,353,216 | 4,607,182,418,800,017,408 
1 to 2 | 8,388,608 | 4,503,599,627,370,496 
2 to 3 | 4,194,304 | 2,251,799,813,685,248 
4 to 5 | 2,097,152 | 1,125,899,906,842,624 
8 to 9 | 1,048,576 | 562,949,953,421,312 
16 to 17 | 524,288 | 281,474,976,710,656 
32 to 33 | 262,144 | 140,737,488,355,328 
64 to 65 | 131,072 | 70,368,744,177,664 
128 to 129 | 65,536 | 35,184,372,088,832 
256 to 257 | 32,768 | 17,592,186,044,416 
512 to 513 | 16,384 | 8,796,093,022,208 
1,024 to 1,025 | 8,192 | 4,398,046,511,104 
2,048 to 2,049 | 4,096 | 2,199,023,255,552 
4,096 to 4,097 | 2,048 | 1,099,511,627,776 
8,192 to 8,193 | 1,024 | 549,755,813,888 
16,384 to 16,385 | 512 | 274,877,906,944 
32,768 to 32,769 | 256 | 137,438,953,472 
65,536 to 65,537 | 128 | 68,719,476,736 
131,072 to 131,073 | 64 | 34,359,738,368 
262,144 to 262,145 | 32 | 17,179,869,184 
524,288 to 524,289 | 16 | 8,589,934,592 
1,048,576 to 1,048,577 | 8 | 4,294,967,296 
2,097,152 to 2,097,153 | 4 | 2,147,483,648 
4,194,304 to 4,194,305 | 2 | 1,073,741,824 
8,388,608 to 8,388,609 | 1 | 536,870,912 
16,777,216 to 16,777,217 | 0 | 268,435,456 
... | ... | ...

As you can see, the extra 29 bits in the mantissa of the double-precision floating-point gives the value a lot more precision. When you are using floating-point values in code, remember that if precision is key, try to keep your values small. If you need an extra boost in precision, use a double-precision value!

## Floating-Point Range

The range of a floating-point value can be broken up into two sections: the range of positive normalized and denormalized values, and the range of negative normalized and denormalized values. If a positive or negative value ever becomes to small to be represented, a quiet exception is thrown for an underflow. Likewise, if a positive or negative value ever becomes too large to be represented by a floating-point value, it will throw a quiet exception for an overflow, and become ±Infinity. 

Here are the range values for a single-precision floating-point value:

```
Largest positive number:
Normalized
0 11111110 11111111111111111111111
3.4028235E38

Smallest positive number:
Denormalized
0 00000000 00000000000000000000001
1.4E-45

Largest negative number:
Denormalized
1 00000000 00000000000000000000001
-1.4E-45

Smallest negative number:
Normalized
1 11111110 11111111111111111111111
-3.4028235E38
```

And here are the range values for a double-precision floating-point value:

```
Largest positive number:
Normalized
0 11111111110 1111111111111111111111111111111111111111111111111111
1.79769E308

Smallest positive number:
Denormalized
0 00000000000 0000000000000000000000000000000000000000000000000001
4.94066E324

Largest negative number:
Denormalized
1 00000000000 0000000000000000000000000000000000000000000000000001
-4.94066E324

Smallest negative number:
Normalized
1 11111111110 1111111111111111111111111111111111111111111111111111
-1.79769E308
```

## Special Values

### Zero

Since the mantissa has an implicit value of 1 before the radix point, 0 cannot be normally represented, so we need a special representation of the value 0. If the exponent and mantissa are all 0, the floating point value will result as a 0.

### Denormalized

Denormalized floating-point values are used to represent _very_ small numbers. Instead of an implicit '1.' in the mantissa in normalized values, we have an implicit '0.' for denormalized values.

This special value is represented by having an exponent of all 0's, and a non-zero mantissa. Although the difference of the bias and an all 0 exponent is -127, denormalized values always have an exponent of -126.

### Infinity

Floating-point values can achieve the value Infinity if any operations cause it to overflow. This is a compromise to gracefully handle becoming too large or too small of a value, which is in my opinion better than how integers handle overflowing by wrapping around. 

The special value for infinity is represented by an exponent of 255 (all 1's) and a mantissa of all 0's. The sign bit is still used to differentiate between positive and negative infinity.

### Not a Number

The Not a Number (or NaN) value is used to represent a value that does not represent a real number. Represented by an exponent of all 1's, and a non-zero mantissa. Although NaN's don't represent any number in particular, they are signed values, meaning the 'sign' bit can be toggled between positive NaN and negative NaN.

There are 2 categories of NaN: the Quiet NaN, and the Signalling NaN.

**Quiet NaN**

The Quiet NaN (or QNaN) is returned from an operation where the result is not mathematically defined. QNaN's are represented by an exponent of all 1's with the most significant mantissa bit set. 

```
Quiet NaN: 0 11111111 10000000000000000000000
```

An example of a QNaN in code would be taking the square root of a negative number. Since this isn't technically possible, it needs to return an indeterminate result. Enter the QNaN!

```
void quietNaN()
{
  cout << sqrt(-1) << endl;
}

Output: -NaN
```

QNaN's won't stop the program from running from an exception, but will help debugging and error handling.

**Signalling NaN**

The Signalling NaN (or SNaN) is used when an operation is invalid and cannot be continued from, and signals an exception. SNaN's are represented by en exponent of all 1's with the most significant bit clear.

```
Signalling NaN: 0 11111111 00000000000000000000000
```

SNaN's can be used as a default value for floats to prevent premature usage of data that shouldn't be used yet. As far as I know, C++ doesn't use SNaN's anywhere in it's spec.

### Operation Results With Special Values

It's important to remember that operating on special values won't always return exactly what you expect. Here is a list of operations with special values, and what they return:

Operation | Result
--- | ---
n / infinity | 0
infinity * infinity | infinity
nonzero / 0 | infinity
infinity + infinity | infinity
0 / 0 | NaN
infinity - infinity | NaN
infinity / infinity | NaN
infinity * 0 | NaN
Any operation with a NaN | NaN
