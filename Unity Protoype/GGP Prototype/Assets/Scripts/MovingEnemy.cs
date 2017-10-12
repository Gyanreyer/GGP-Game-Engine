using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[RequireComponent(typeof(Enemy))]
public class MovingEnemy : MonoBehaviour
{
    Vector3 startPt;

	// Use this for initialization
	void Start () {
        startPt = transform.position;
	}
	
	// Update is called once per frame
	void Update () {
        transform.position = startPt + transform.right * Mathf.Sin(Time.time * 1.25f) * 3;	
	}
}
