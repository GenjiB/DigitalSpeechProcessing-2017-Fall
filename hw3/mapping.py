import sys
import pdb



def main(argv):
    dic = {}
    #read
    with open(argv[1], 'r', encoding='BIG5-HKSCS') as fp:
        for line in fp:
            word , JuYin_sentence = line.split(' ')
            JuYin = JuYin_sentence.split('/')
            for j in JuYin:
                if j[0] in dic:
                    dic[j[0]].append(word)
                else:
                    dic[j[0]] = [word]
            dic[word] = [word]
    #write
    with open(argv[2], 'w', encoding='BIG5-HKSCS') as out:
        for key in sorted(dic.keys()):
            out.write(key)
            for word in dic[key]:
                out.write(' '+word)
            out.write('\n')
if __name__ == '__main__':
    main(sys.argv)
