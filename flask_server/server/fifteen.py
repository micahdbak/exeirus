def read_file (filename):
    # Output array
    init_vals = []
    file = open(filename, "r")
    # Pass all strings into the array
    for line in file:
        init_vals.append(line)

    # Break string into numbers and map to integers
    splitted = init_vals[1].split(" ")
    init_vals[1] = list( map( int,  splitted) )
    # Remove all spaces for the key string
    init_vals[2] = init_vals[2].replace(" ", "")

    return init_vals
