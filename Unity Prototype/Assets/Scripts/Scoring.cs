using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Scoring : MonoBehaviour
{
    private int score;
    private int numEnemies;
    private Canvas hudCanvas;
    private UnityEngine.UI.Text scoreBox;
    private UnityEngine.UI.Text remainingBox;
    private UnityEngine.UI.Text winBox;

	// Use this for initialization
	void Start () {
        score = 0; //Set the score
        
        //Get the number of enemies in possibly the most compact (and to some, dumbest) way possible
        foreach (GameObject enemies in GameObject.FindGameObjectsWithTag("target"))
            numEnemies++;

        hudCanvas = FindObjectOfType<Canvas>(); //Get the canvas

        UnityEngine.UI.Text[] hudTextBoxes = hudCanvas.GetComponentsInChildren<UnityEngine.UI.Text>(); //Get all the text elements from the canvas

        foreach (UnityEngine.UI.Text textBox in hudTextBoxes)
        {
            //Get the score box to update and display the initial score
            if (textBox.name == "Count")
            {
                scoreBox = textBox;
                scoreBox.text = score.ToString();
            }
            //Get the win box and disable it
            else if (textBox.name == "RCount")
            {
                remainingBox = textBox;
                remainingBox.text = numEnemies.ToString();
            }
            //Get the win box and disable it
            else if (textBox.name == "Win")
            {
                winBox = textBox;
                winBox.enabled = false;
            }
        }
	}
	
    //Increment and display the score
	public void IncrementScore(int amount)
    {
        //Update the score
        score += amount;
        scoreBox.text = score.ToString();

        //Update the number of remaining enemies
        numEnemies--;
        remainingBox.text = numEnemies.ToString();

        //If the player has won
        if (numEnemies <= 0)
            winBox.enabled = true;
    }
}
