using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Bullet : MonoBehaviour
{
    [SerializeField] private GameObject shooter;
    private float speed = .25f; //The speed of the bullet

    void FixedUpdate() //Physics
    {
        transform.Translate(shooter.transform.forward * speed);
    }

    private void OnCollisionEnter(Collision coll) //Colliding with objects
    {
        if (coll.gameObject.tag == "target")
        {
            Destroy(coll.gameObject); //Destroy target
            Destroy(gameObject); //Destroy bullet
        }
        else if (coll.gameObject.tag == "ground")
            Destroy(gameObject); //Destroy bullet
    }
}