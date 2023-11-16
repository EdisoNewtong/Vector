# Markdown Cheat Sheet

Thanks for visiting [The Markdown Guide](https://www.markdownguide.org)!

This Markdown cheat sheet provides a quick overview of all the Markdown syntax elements. It can’t cover every edge case, so if you need more information about any of these elements, refer to the reference guides for [basic syntax](https://www.markdownguide.org/basic-syntax) and [extended syntax](https://www.markdownguide.org/extended-syntax).

## Basic Syntax

These are the elements outlined in John Gruber’s original design document. All Markdown applications support these elements.

### Heading

# H1

H1 Content Line 1
H1 Content Line 2
H1 Content Line 3

## H2

H2 Content Line 1
H2 Content Line 2
H2 Content Line 3

### H3

#### Bold

**bold text**

#### Italic

*italicized text*

#### Bold && Italic

***bold text***

### Blockquote

> blockquote

### Ordered List

1.  First item
2.  Second item
3.  Third item

### Unordered List

*   First item
*   Second item
*   Third item

### Code

#### single line code

`int a = x*x*x;`

### Horizontal Rule

***

### Link

[Markdown Guide](https://www.markdownguide.org)

### Image

![alt text](file:///M:/Color_Mode.png)

## Extended Syntax

These elements extend the basic syntax by adding additional features. Not all Markdown applications support these elements.

### Table

| Syntax    | Description |
| --------- | ----------- |
| Header    | Title       |
| Paragraph | Text        |

### Fenced Code Block

```JavaScript
{
  "firstName": "John",  
  "lastName": "Smith",   
  "age": 25
}
```

### Footnote

Here's a sentence with a footnote. [^1]

[^1]: This is the footnote.

### Heading ID

### My Great Heading {#custom-id}

### Definition List

term
:   definition

### Strikethrough

~~The world is flat.~~

### Task List

*   [x] Write the press release
*   [ ] Update the website
*   [ ] Contact the media

### Emoji

That is so funny! \:joy:

(See also [Copying and Pasting Emoji](https://www.markdownguide.org/extended-syntax/#copying-and-pasting-emoji))

### Highlight

I need to highlight these ==very important words== .

### Subscript

H~2~O

### Superscript

X^2^

### Color Text &#x20;

\*\*Not All MarkDown Editor support the following syntax\*\*&#x20;

<font color="blue">Blue Text</font> <font color="#FF00FF">Pink Text</font>

### ==Characters You Can Escape==

You can use a backslash to escape the following characters.

| Character | Name                                             |
| --------- | ------------------------------------------------ |
| \\        | backslash                                        |
| \`        | backtick ( see also escaping backticks in code ) |
| \*        | arterisk                                         |
| \_        | underscore                                       |
| { }       | curly braces                                     |
| \[ ]      | brackets                                         |
| < >       | angle brackets                                   |
| ( )       | parentheses                                      |
| #         | pund sign                                        |
| +         | plus sign                                        |
| -         | minus sign ( hyphen )                            |
| .         | dot                                              |
| !         | exclamation mark                                 |
| \|        | pipe ( see also escaping pipe in tables )        |

