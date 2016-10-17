/*
 * Topics : Naive Bayes Text classification
 * Stunent ID: 0905060
 * Machine Learning Lab
 * Date: 25/04/2015
 */

package mloffline2;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author chumki
 */
public class Main {

    int modV =  0;
    static int total_docs=0;
    static HashMap<String, HashMap<String,Integer>> data = new HashMap<String, HashMap<String, Integer>>();
    static HashMap<String,Integer>word_count = new HashMap<String, Integer>();
    static HashMap<String,Integer>class_count = new HashMap<String, Integer>();
    static HashMap<String,Double>priors = new HashMap<String, Double>();
    static HashMap<String, HashMap<String,Double>>conditionals = new HashMap<String, HashMap<String, Double>>();
    static HashMap<String,Double>probs = new HashMap<String, Double>();//for test
    static HashMap<String, Integer> class_word_count = new HashMap<String, Integer>();//stores words count of a class
    static int correct=0;
    static int total=0;
    //static String delim = "[\\s,.]+";
    static String delim = " ";
    static String[] avoid_string = {"The","in","the","REUTER","of","by","with"," ",",",".",";","-",":","on","at","to","from","for","A","a","I","And","and","an"};
    /**
     * @param args the command line arguments
     */

    public static void main(String[] args) throws IOException {
        File fin = new File("training.data");
        FileInputStream fis = new FileInputStream(fin);
 
	//Construct BufferedReader from InputStreamReader
	BufferedReader br = new BufferedReader(new InputStreamReader(fis));
 
	String line = null;
        int newline_count=0;
        String type="";

	while ((line = br.readLine()) != null) {
              if(!line.equals("") && newline_count==0)
              {
                  total_docs++;
                  class_count.put(line, class_count.get(line)==null?1:class_count.get(line)+1);
                  type=line;
                  //System.out.println(type);
              }
            else if (!line.equals(""))
                count_word(type,line);

            else if(line.equals(""))
                     newline_count = (newline_count + 1) % 5;


                

        }
        calculate_prior();
        print_priors();

        count_words_in_class();
        calculate_conditionls();
br.close();
        test();

        System.out.println("Accuracy "+(double)((double)correct/(double)total)*100);

        System.out.println("lenght of vector : "+word_count.size());
       // System.out.println("total docs are: "+ total_docs+" "+data.size()+" "+class_count.size());
        //System.out.println(" "+conditionals.get("be").get("stg"));
	
       

    }
    static void count_word(String type,String str)
    {
            if(data.get(type)==null)
            data.put(type, new HashMap<String, Integer>());

            HashMap<String,Integer> temp = data.get(type);

           // String[] words = str.split("[\\p{Punct}\\s]+");
            String[] words = str.split(delim);
            for(int i=0;i<words.length;i++)
            {
                if(!toAvoid(words[i]))
                {
                    word_count.put(words[i], word_count.get(words[i]) == null ? 1 : word_count.get(words[i]) + 1);
                    temp.put(words[i], temp.get(words[i])==null?1:temp.get(words[i])+1);
                }
            }
            data.put(type, temp);

    }

    static void calculate_prior()
    {
        // Get a set of the entries
      Set set = class_count.entrySet();
      // Get an iterator
      Iterator i = set.iterator();
      // Display elements
      while(i.hasNext()) {
         Map.Entry me = (Map.Entry)i.next();
         priors.put( (String)me.getKey(), (double) ((double)class_count.get(me.getKey())/(double)total_docs));
        // System.out.print(me.getKey() + ": ");
        // System.out.println(me.getValue());
      }
    }

    static void print_priors()
    {
       Set set = priors.entrySet();
      // Get an iterator
      Iterator i = set.iterator();
      // Display elements
      while(i.hasNext()) {
         Map.Entry me = (Map.Entry)i.next();
        // priors.put( (String)me.getKey(), (double) ((double)class_count.get(me.getKey())/(double)total_docs));
         System.out.print(me.getKey() + ": ");
         System.out.println(me.getValue());
      }
    }
    static void calculate_conditionls()
    {
        Set set = word_count.entrySet();
      // Get an iterator
      Iterator i = set.iterator();
      // Display elements
      while(i.hasNext()) {
         Map.Entry me = (Map.Entry)i.next();
         String word = (String)me.getKey();

            Set set2 = data.entrySet();
          // Get an iterator
          Iterator i2 = set2.iterator();

          // Display elements
          while(i2.hasNext())
          {
              Map.Entry me2 = (Map.Entry)i2.next();
              String type = (String)me2.getKey();
              double prob;

                prob = (double)(data.get(type).get(word)==null?1.0:(double)data.get(type).get(word)+1.0)/(double)(class_word_count.get(type)+(double)word_count.size());
               // prob = (double)(data.get(type).get(word)==null?1.0:(double)data.get(type).get(word)+(double)1)/(double)(100+(double)word_count.size());
             if(conditionals.get(word)==null)
              conditionals.put(word, new HashMap<String, Double>());
             HashMap<String,Double>temp = conditionals.get(word);

             temp.put(type, prob);
             conditionals.put(word, temp);

             //System.out.print("#");

          }
      }
    }


    static double count_words_in_class()
    {

        int count=0;

        Set x = data.entrySet();
        Iterator xi = x.iterator();
        while(xi.hasNext()){
            Map.Entry y = (Map.Entry)xi.next();

            String type = (String)y.getKey();

           Set set = data.get(type).entrySet();
          // Get an iterator
          Iterator i = set.iterator();
          while(i.hasNext())
          {
               Map.Entry me = (Map.Entry)i.next();
               String word = (String)me.getKey();
               count+=data.get(type).get(word);
          }
          
          class_word_count.put(type, count);
          count=0;

        }
          return (double)count;

    }

    static void test()throws IOException
    {
        File fin = new File("test.data");
        FileInputStream fis2 = new FileInputStream(fin);
 
	//Construct BufferedReader from InputStreamReader
	BufferedReader br2 = new BufferedReader(new InputStreamReader(fis2));
 
	String line = null;
        int newline_count=0;
        String type="";
        
	while ((line = br2.readLine()) != null) {
              if(!line.equals("") && newline_count==0)
              {
                  type=line;
                  //System.out.println(type);
              }
            else if (!line.equals(""))
                count_probs1(line);
              else if(newline_count==4)
              {
                  if(line.equals(""))newline_count=(newline_count+1)%5;
                  finalize_count_probs(type);
              }

            else if(line.equals(""))
                     newline_count = (newline_count + 1) % 5;


                

        }
    }
    static void count_probs1(String line)
    {
        //String[] words = line.split("[\\p{Punct}\\s]+");
        String[] words = line.split(delim);
          
        for(int i=0;i<words.length;i++)
        {
             Set set2 = class_count.entrySet();
          // Get an iterator
          Iterator i2 = set2.iterator();
          
          while(i2.hasNext())
          {
              Map.Entry me = (Map.Entry)i2.next();
              String type = (String)me.getKey();
               if(!toAvoid(words[i]))
                {
                     if(conditionals.get(words[i])!=null )
                    if(conditionals.get(words[i]).get(type)!=null)
                    {
                          // if(conditionals.get(words[i]).get(type)==0.0)System.out.println("@@@");

                     probs.put(type, probs.get(type)==null?Math.log(conditionals.get(words[i]).get(type)) : probs.get(type)+Math.log(conditionals.get(words[i]).get(type)));
                    //if(conditionals.get(words[i]).get(type)==0.0)System.out.println("0");
                      //if(probs.get(type)==0.0)System.out.println("0 "+words[i]+" "+conditionals.get(words[i]).get(type)+" "+type);
                  
                    }
              }
          }

        }
    }
    
    static void finalize_count_probs(String Type)
    {
         Set set2 = class_count.entrySet();
          // Get an iterator
          Iterator i2 = set2.iterator();
          while(i2.hasNext())
          {
              Map.Entry me = (Map.Entry)i2.next();
              String type = (String)me.getKey();
              probs.put(type, probs.get(type)==null?Math.log(priors.get(type)):probs.get(type)+Math.log(priors.get(type)));
          
          }
          
          if(get_max_prob().equals(Type))
          {
              System.out.println("Correct!!! Expected: "+Type+" Found: "+get_max_prob());
              correct++;
          }
          else
          {
             System.out.println("Wrong!!!!! Expected: "+get_max_prob()+" Found: "+Type);
          }
          total++;
          probs.clear();
     

    }
    
    static String get_max_prob()
    {
         Set set2 = probs.entrySet();
          // Get an iterator
          Iterator i2 = set2.iterator();
          double max = -100000.0;
          String max_type="";
          while(i2.hasNext())
          {
              Map.Entry me = (Map.Entry)i2.next();
              String type = (String)me.getKey();
             // System.out.println("-- "+probs.get(type));
              if(probs.get(type)>max)
              {
                  max=probs.get(type);
                  max_type = type;
              }
          }
          return max_type;
    }
    static boolean toAvoid(String word)
            {

        for(int i=0;i<avoid_string.length;i++)
        {
            if(word.equals(avoid_string[i]))return true;
        }
        return false;
    }
}
